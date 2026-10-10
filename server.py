import http.server
import socketserver
import json
import subprocess
import os

PORT = 5050
EXECUTABLE = "./sangam_core.exe" if os.name == 'nt' else "./sangam_core"

class Handler(http.server.SimpleHTTPRequestHandler):
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
                capture_output=True, text=True, check=True
            )
            return result.stdout.strip()
        except subprocess.CalledProcessError as e:
            return e.stdout.strip() if e.stdout else '{"error": "Internal Error"}'

    def do_GET(self):
        req_data = None
        if self.path == '/api/health' or self.path == '/health':
            req_data = {"action": "health"}
        elif self.path == '/api/places' or self.path == '/places':
            req_data = {"action": "get_places"}
        elif self.path == '/api/categories' or self.path == '/categories':
            req_data = {"action": "get_categories"}
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
        elif self.path == '/api/itineraries' or self.path == '/itineraries':
            req_data = {"action": "get_itineraries"}
        
        if req_data:
            response = self.run_cpp(req_data)
            self.send_response(200)
            self.send_header('Content-Type', 'application/json')
            self.end_headers()
            self.wfile.write(response.encode('utf-8'))
        else:
            # serve static files if needed, or fallback
            if self.path == '/' or self.path == '/index.html':
                self.path = '/webpage/index.html'
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

if __name__ == '__main__':
    with socketserver.TCPServer(("", PORT), Handler) as httpd:
        print(f"Serving at port {PORT}")
        httpd.serve_forever()
