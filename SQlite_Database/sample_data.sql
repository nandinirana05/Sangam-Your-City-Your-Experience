PRAGMA foreign_keys = ON;

-- USERS

INSERT INTO users (id, name, username, email, password_hash) VALUES
(1, 'Nandini Rana', 'nandini', 'nandini@sangam.com', 'hash_nandini'),
(2, 'Srishti Dhasmana', 'srishti', 'srishti@sangam.com', 'hash_srishti'),
(3, 'Ishita Bijalwan', 'ishita', 'ishita@sangam.com', 'hash_ishita'),
(4, 'Somya Dhoundiyal', 'somya', 'somya@sangam.com', 'hash_somya'),
(5, 'Vidisha Dobhal', 'vidisha', 'vidisha@sangam.com', 'hash_vidisha'),
(6, 'Kajal Goel', 'kajal', 'kajal@sangam.com', 'hash_kajal'),
(7, 'Soma Rawat', 'soma', 'soma@sangam.com', 'hash_soma'),
(8, 'Anvi Dhyani', 'anvi', 'anvi@sangam.com', 'hash_anvi'),
(9, 'Srijan Dhasmana', 'srijan', 'srijan@sangam.com', 'hash_srijan'),
(10, 'Sara Raturi', 'sara', 'sara@sangam.com', 'hash_sara'),
(11, 'Vansika Negi', 'vansika', 'vansika@sangam.com', 'hash_vansika'),
(12, 'Aryaman Bhatnagar', 'aryaman', 'aryaman@sangam.com', 'hash_aryaman'),
(13, 'Swatantra Chaudhary', 'swatantra', 'swatantra@sangam.com', 'hash_swatantra'),
(14, 'Abhyudit Sharma', 'abhyudit', 'abhyudit@sangam.com', 'hash_abhyudit'),
(15, 'Atharva Madan', 'atharva', 'atharva@sangam.com', 'hash_atharva');


-- PROFILES

INSERT INTO profiles (id, user_id, bio, profile_picture) VALUES
(1, 1, 'Vlogging through life', 'https://example.com/profiles/nandini.jpg'),
(2, 2, 'Travel, food and hidden places.', 'https://example.com/profiles/srishti.jpg'),
(3, 3, 'Coffee, mountains and good views.', 'https://example.com/profiles/ishita.jpg'),
(4, 4, 'Always looking for new places.', 'https://example.com/profiles/somya.jpg'),
(5, 5, 'Local explorer and photographer.', 'https://example.com/profiles/vidisha.jpg'),
(6, 6, 'Wandelust', 'https://example.com/profiles/kajal.jpg'),
(7, 7, 'Travel enthusiast.', 'https://example.com/profiles/soma.jpg'),
(8, 8, 'Explorer and photographer.', 'https://example.com/profiles/anvi.jpg'),
(9, 9, 'Exploring the world one step at a time.', 'https://example.com/profiles/srijan.jpg'),
(10, 10, 'Hi Guys. Welcome to my channel!', 'https://example.com/profiles/sara.jpg'),
(11, 11, 'Chasing the clouds', 'https://example.com/profiles/vansika.jpg'),
(12, 12, 'If i could fly', 'https://example.com/profiles/aryaman.jpg'),
(13, 13, 'Country roads take me home', 'https://example.com/profiles/swatantra.jpg'),
(14, 14, 'Travel, food and hidden places.', 'https://example.com/profiles/abhyudit.jpg'),
(15, 15, 'Full time traveller', 'https://example.com/profiles/atharva.jpg');


-- FOLLOWERS

INSERT INTO followers (id, follower_id, following_id) VALUES
(1, 1, 2),
(2, 1, 3),
(3, 2, 1),
(4, 2, 3),
(5, 3, 1),
(6, 3, 2),
(7, 4, 5),
(8, 5, 4),
(9, 6, 7),
(10, 7, 6),
(11, 8, 9),
(12, 9, 8),
(13, 10, 11),
(14, 11, 10),
(15, 12, 13),
(16, 13, 12),
(17, 14, 15),
(18, 15, 14);


-- PLACES
-- category IDs:
-- 1 Restaurant
-- 2 Cafe
-- 3 Park
-- 4 Museum
-- 5 Shopping Mall
-- 6 Hotel
-- 7 Theater
-- 8 Gym
-- 9 Library


INSERT INTO places
(id, user_id, category_id, name, description, latitude, longitude)
VALUES
(1, 1, 3, 'Robber''s Cave', 
 'A famous natural cave and waterfall destination near Dehradun.',
 30.3715, 78.0770),

(2, 2, 3, 'Malsi Deer Park',
 'A peaceful nature park located near the foothills of Mussoorie.',
 30.3724, 78.0752),

(3, 3, 2, 'Café de Piccolo',
 'A cozy cafe for coffee, snacks and relaxed conversations.',
 30.3256, 78.0437),

(4, 4, 4, 'Forest Research Institute',
 'A historic research institution known for its colonial architecture and museum.',
 30.3426, 77.9996),

(5, 5, 5, 'Pacific Mall Dehradun',
 'A popular shopping and entertainment destination.',
 30.3540, 78.0810),

(6, 1, 1, 'Kabila Restaurant',
 'A casual restaurant serving a variety of Indian dishes.',
 30.3165, 78.0322),

(7, 2, 6, 'Hotel Madhuban',
 'A comfortable hotel located in central Dehradun.',
 30.3340, 78.0550),

(8, 3, 3, 'Lachhiwala Nature Park',
 'A popular outdoor destination surrounded by greenery and water pools.',
 30.2297, 78.1050),

(9, 4, 9, 'Dehradun Central Library',
 'A quiet place for reading and studying.',
 30.3165, 78.0320),

(10, 5, 2, 'First Cup Coffee',
 'A casual cafe popular for coffee and quick snacks.',
 30.3250, 78.0430);


-- CONNECTIONS
-- Used for Dijkstra / route finding

INSERT INTO connections
(id, from_place_id, to_place_id, distance_km, travel_time_min)
VALUES
(1, 1, 2, 1.8, 6),
(2, 2, 3, 5.2, 15),
(3, 3, 4, 6.0, 18),
(4, 4, 5, 7.5, 22),
(5, 5, 6, 5.1, 16),
(6, 6, 7, 4.0, 12),
(7, 7, 8, 8.5, 25),
(8, 8, 9, 12.0, 30),
(9, 9, 10, 2.5, 8),
(10, 3, 10, 1.2, 5),
(11, 1, 4, 7.0, 20),
(12, 4, 8, 9.0, 27);


-- PLACE IMAGES

INSERT INTO place_images
(id, place_id, user_id, image_url)
VALUES
(1, 1, 1, 'https://example.com/places/robbers-cave-1.jpg'),
(2, 1, 2, 'https://example.com/places/robbers-cave-2.jpg'),
(3, 2, 3, 'https://example.com/places/malsi-deer-park-1.jpg'),
(4, 3, 4, 'https://example.com/places/cafe-de-piccolo-1.jpg'),
(5, 4, 5, 'https://example.com/places/fri-1.jpg'),
(6, 5, 1, 'https://example.com/places/pacific-mall-1.jpg'),
(7, 6, 2, 'https://example.com/places/kabila-restaurant-1.jpg'),
(8, 7, 3, 'https://example.com/places/hotel-madhuban-1.jpg'),
(9, 8, 4, 'https://example.com/places/lachhiwala-nature-park-1.jpg'),
(10, 9, 5, 'https://example.com/places/dehradun-central-library-1.jpg'),
(11, 10, 1, 'https://example.com/places/first-cup-coffee-1.jpg'),
(12, 10, 2, 'https://example.com/places/first-cup-coffee-2.jpg');


-- PLACE VIDEOS

INSERT INTO place_videos
(id, place_id, user_id, video_url)
VALUES
(1, 1, 1, 'https://example.com/videos/robbers-cave.mp4'),
(2, 4, 4, 'https://example.com/videos/fri.mp4'),
(3, 8, 3, 'https://example.com/videos/lachhiwala.mp4'),
(4, 5, 5, 'https://example.com/videos/pacific-mall.mp4'),
(5, 3, 2, 'https://example.com/videos/cafe-de-piccolo.mp4'),
(6, 6, 1, 'https://example.com/videos/kabila-restaurant.mp4');


-- PLACE REVIEWS

INSERT INTO place_reviews
(id, place_id, user_id, rating, review)
VALUES
(1, 1, 1, 5, 'Beautiful place and great for a short outing.'),
(2, 1, 2, 4, 'Nice experience, especially during the cooler months.'),
(3, 2, 3, 4, 'Peaceful and surrounded by greenery.'),
(4, 3, 4, 5, 'Great coffee and a really cozy atmosphere.'),
(5, 4, 5, 5, 'The architecture is impressive and the museum is interesting.'),
(6, 5, 1, 4, 'Good place for shopping and entertainment.'),
(7, 8, 2, 4, 'Nice place to spend a few hours outdoors.'),
(8, 10, 3, 4, 'Good coffee and a comfortable atmosphere.');


-- SAVED PLACES

INSERT INTO saved_places
(id, user_id, place_id)
VALUES
(1, 1, 4),
(2, 1, 8),
(3, 2, 1),
(4, 2, 5),
(5, 3, 3),
(6, 3, 10),
(7, 4, 1),
(8, 5, 4),
(9, 6, 2),
(10, 7, 3),
(11, 8, 5),
(12, 9, 6),
(13, 10, 7),
(14, 11, 8),
(15, 12, 9),
(16, 13, 10),
(17, 14, 1),
(18, 15, 2);


-- ITINERARIES

INSERT INTO itineraries
(id, user_id, name, description)
VALUES
(1, 1, 'Dehradun Nature Day',
 'A one-day itinerary covering some of Dehradun''s natural attractions.'),

(2, 2, 'Dehradun Explorer',
 'A mix of sightseeing, food and shopping.'),

(3, 3, 'Relaxed Dehradun',
 'A slower itinerary focused on cafes and peaceful places.');


-- ITINERARY PLACES

INSERT INTO itinerary_places
(id, itinerary_id, place_id, visit_order)
VALUES
(1, 1, 1, 1),
(2, 1, 2, 2),
(3, 1, 8, 3),

(4, 2, 4, 1),
(5, 2, 6, 2),
(6, 2, 5, 3),

(7, 3, 3, 1),
(8, 3, 10, 2),
(9, 3, 4, 3);


-- POSTS

INSERT INTO posts
(id, user_id, title, content)
VALUES
(1, 1, 'Hidden Gems of Dehradun',
 'Some of my favourite places to explore around Dehradun.'),

(2, 2, 'Best Places for a Weekend',
 'Here are a few places that are worth visiting if you have a free weekend.'),

(3, 3, 'Coffee Spots in Dehradun',
 'A small list of cafes that are perfect for coffee and conversations.'),

(4, 4, 'Exploring FRI',
 'The architecture and surroundings of FRI are absolutely worth seeing.'),
 
 (5, 5, 'Shopping in Dehradun',
 'A guide to some of the best shopping destinations in Dehradun.');


-- COMMENTS

INSERT INTO comments
(id, post_id, user_id, content)
VALUES
(1, 1, 2, 'Robber''s Cave is definitely worth visiting.'),
(2, 1, 3, 'Adding this to my list!'),
(3, 2, 1, 'The FRI campus is beautiful.'),
(4, 2, 5, 'Lachhiwala is also a good option.'),
(5, 3, 4, 'I need to try these cafes.'),
(6, 4, 3, 'The architecture looks amazing.'),
(7, 5, 9, 'Pacific Mall has some great stores.');


-- LIKES

INSERT INTO likes
(id, post_id, user_id)
VALUES
(1, 1, 2),
(2, 1, 3),
(3, 1, 4),
(4, 2, 1),
(5, 2, 11),
(6, 3, 4),
(7, 3, 5),
(8, 4, 1),
(9, 4, 3),
(11, 5, 9),
(12, 5, 10);