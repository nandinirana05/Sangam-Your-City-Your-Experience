import http.server
import json
import subprocess
import os
import socket

PORT = 5050
BASE_DIR = os.path.dirname(os.path.abspath(__file__))
WEBPAGE_DIR = os.path.join(BASE_DIR, 'webpage')
EXECUTABLE = os.path.join(BASE_DIR, "sangam_core.exe" if os.name == 'nt' else "sangam_core")

# In-memory cache for static data
_cache = {}

class Handler(http.server.SimpleHTTPRequestHandler):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, directory=WEBPAGE_DIR, **kwargs)

    def address_string(self):
        # Return IP directly without reverse DNS lookup
        return self.client_address[0]

    def end_headers(self):
        self.send_header('Access-Control-Allow-Origin', '*')
        self.send_header('Access-Control-Allow-Methods', 'GET, POST, OPTIONS')
        self.send_header('Access-Control-Allow-Headers', 'Content-Type')
        super().end_headers()

    def do_OPTIONS(self):
        self.send_response(200)
        self.end_headers()

    def run_cpp(self, req_data):
        try:
            result = subprocess.run(
                [EXECUTABLE, json.dumps(req_data)],
                cwd=BASE_DIR,
                capture_output=True, text=True, check=True
            )
            return result.stdout.strip()
        except subprocess.CalledProcessError as e:
            return e.stdout.strip() if e.stdout else '{"error": "Internal Error"}'

    def run_cpp_cached(self, cache_key, req_data):
        if cache_key not in _cache:
            _cache[cache_key] = self.run_cpp(req_data)
        return _cache[cache_key]

    def log_message(self, format, *args):
        # Keep server quiet and fast
        pass

    def do_GET(self):
        req_data = None
        cache_key = None
        if self.path in ('/api/health', '/health'):
            req_data = {"action": "health"}
        elif self.path in ('/api/places', '/places'):
            req_data = {"action": "get_places"}
            cache_key = "places"
        elif self.path in ('/api/categories', '/categories'):
            req_data = {"action": "get_categories"}
            cache_key = "categories"
        elif self.path.startswith('/api/search') or self.path.startswith('/search'):
            query = ""
            if "?" in self.path:
                qs = self.path.split("?")[1]
                for p in qs.split("&"):
                    if p.startswith("q="):
                        query = p.split("=")[1]
            import urllib.parse
            query = urllib.parse.unquote(query)
            req_data = {"action": "search", "q": query}
        elif self.path in ('/api/itineraries', '/itineraries'):
            req_data = {"action": "get_itineraries"}
        
        if req_data:
            response = self.run_cpp_cached(cache_key, req_data) if cache_key else self.run_cpp(req_data)
            self.send_response(200)
            self.send_header('Content-Type', 'application/json')
            self.end_headers()
            self.wfile.write(response.encode('utf-8'))
        else:
            super().do_GET()

    def do_POST(self):
        content_length = int(self.headers.get('Content-Length', 0))
        body = self.rfile.read(content_length).decode('utf-8')
        try:
            data = json.loads(body)
        except:
            data = {}

        req_data = None
        if self.path == '/api/route/optimize':
            data["action"] = "optimize_route"
            req_data = data
        elif self.path == '/api/itineraries':
            data["action"] = "save_itinerary"
            req_data = data
            
        if req_data:
            response = self.run_cpp(req_data)
            self.send_response(200)
            self.send_header('Content-Type', 'application/json')
            self.end_headers()
            self.wfile.write(response.encode('utf-8'))
        else:
            self.send_response(404)
            self.end_headers()

class DualStackThreadingServer(http.server.ThreadingHTTPServer):
    address_family = socket.AF_INET6
    daemon_threads = True

    def server_bind(self):
        # Enable dual-stack IPv4/IPv6 to prevent 2-second Windows DNS timeout
        try:
            self.socket.setsockopt(socket.IPPROTO_IPV6, socket.IPV6_V6ONLY, 0)
        except (AttributeError, OSError):
            pass
        super().server_bind()

if __name__ == '__main__':
    # Pre-cache places and categories on startup
    try:
        h = Handler.__new__(Handler)
        _cache["places"] = h.run_cpp({"action": "get_places"})
        _cache["categories"] = h.run_cpp({"action": "get_categories"})
    except Exception as e:
        pass

    try:
        httpd = DualStackThreadingServer(('::', PORT), Handler)
    except Exception:
        # Fallback to standard IPv4 if IPv6 unsupported
        httpd = http.server.ThreadingHTTPServer(('0.0.0.0', PORT), Handler)

    print(f"Sangam running at http://127.0.0.1:{PORT} and http://localhost:{PORT}")
    with httpd:
        httpd.serve_forever()
