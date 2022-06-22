
/*
* 
*	Jared Commanda
* 
	If you find this on github:
	This "game" was created using the SFML library, with zero knowledge of any game background (so when criticizing, don't flame me for improper strategies)
	This game is tile based, and is pieced together with 32x32 sprites. I created a tile system that can be found in functions titled "generate_______".
	Some of the variables may or may not be needed. This game was rushed in the later development (adding enemies and onwards) and I took about a week break to look at other
	school projects, so the source code was a bit confusing to me and I had no idea on how to clean it up.

	The game "works", but it certainly isn't fun. When doing another project like this I would stick to Unity or another proper game engine.
	I have learned a lot, and if you're a beginner and can understand all the spaghetti code below, you might too


	Known bugs:
	
	Enemies do not respawn. This is because I cannot perform arithmetic operations during a for(int _) loop. It was not letting me and it was near the deadline for submission.
	
	You may only shoot one bullet at a time, and in some cases it may not fire (look at the top left of the screen). The actual bullet sprite was not disappearing and it was
	continuously drawn over and over again each loop. In some cases, the bullet will fire in the top left and nowhere near the actual character. I believe that this error may be
	a result of the error above, where I can't perform arthmetic operations (which is why I state bulletPos = blah blah in the very bottom of the window draw loop.

	The game is very "laggy" when performing multiple operations, this is because I used a method of delaying code using the <chrono> and <thread> iostream libraries.
	After learning a method of setting speed using a elapsed time/slowed framerate solution, I know how to fix it but don't have the time. (This is caused by me not knowing
	common game development practices during the beginning of this project)

	

	Originally this was intended to be a game where you could run and gun enemies, but a tile based system made it hard (I also had no experience with C++ classes and 
	didn't think of storing the x,y coordinates as well as the tile integer position under the tile object). 

*/





/*
Dependencies & Libraries
*/

#include <SFML\Graphics.hpp>
#include <SFML\Audio.hpp>
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <chrono>
#include <map>
#include <vector>
#include <thread>

using namespace std;
using namespace std::this_thread; // sleep_for, sleep_until
using namespace std::chrono; // nanoseconds, system_clock, seconds

/*
Classes
*/

class asset {
public:
	sf::Texture texture;
	std::string identity;
};

class tile {
public:
	std::string identity;
	int location = NULL;
	sf::Vector2f vecLocation;
	int lastMove = 0;
};

class bullet {
public:
	std::string identity;
	sf::Vector2f location;
	sf::Vector2f startLocation;
	int id;
	string direction;
};


/*
Variables
*/

bool debugMode = true;
int charTile = 250;
int newPos; // Ainsley new pos variable
int ainsleySpeed = 100; // Change to decrease or increase the speed that ainsley enemies will move at
int lastMove = 0; // Ainsley last move count
int ainsleyCount; // Variable to store the amount of ainsley's on the screen, if below 10, will respawn an ainsley on a random tile
bool characterDead = false;
bool invertCharacter = false;
bool shotRight = false;
bool shotLeft = false;
bool shootLeft;
bool shootRight;
bool bulletSpawned = false;
bool mainMenu = true; // Main menu variable, set to true on start up
float bulletSpeedConst = 200;
float bulletSpeed = 0;
sf::Vector2f calcPos;
sf::Vector2f charPos = { 0,0 };

std::vector<tile> tiles(1042);
std::vector<tile> groundTiles(1042);
std::vector<bullet> activeBullets(5);

int health = 5;

/*
Functions
*/

std::map<string, asset> loadAssets() {
	std::map<string, asset> assets;

	sf::Texture characterTexture;
	characterTexture.loadFromFile("./dependencies/assets/character.png", sf::IntRect(96, 64, 32, 32));
	characterTexture.setSmooth(true);
	//
	asset* charAsset = new asset;
	charAsset->texture = characterTexture;
	charAsset->identity = "character";
	assets.insert(assets.begin(), pair<string, asset>("character", *charAsset)); // Insert a key-value pair of "character" to correspond with a pointer to charAsset

	//

	sf::Texture groundTexture1;
	groundTexture1.loadFromFile("./dependencies/assets/Pixel Art Top Down - Basic/Texture/TX Tileset Grass.png", sf::IntRect(0, 0, 32, 32));
	groundTexture1.setSmooth(true);
	//
	asset* ground1Asset = new asset;
	ground1Asset->texture = groundTexture1;
	ground1Asset->identity = "blank grass";
	assets.insert(assets.begin(), pair<string, asset>("blank grass", *ground1Asset));

	//

	sf::Texture groundTexture2;
	groundTexture2.loadFromFile("./dependencies/assets/Pixel Art Top Down - Basic/Texture/TX Tileset Grass.png", sf::IntRect(32, 64, 32, 32));
	groundTexture2.setSmooth(true);
	//
	asset* ground2Asset = new asset;
	ground2Asset->texture = groundTexture2;
	ground2Asset->identity = "leaf grass";
	assets.insert(assets.begin(), pair<string, asset>("leaf grass", *ground2Asset));

	//

	sf::Texture ainsleyTexture;
	ainsleyTexture.loadFromFile("./dependencies/assets/ainsley.png", sf::IntRect(0, 0, 32, 32));
	ainsleyTexture.setSmooth(true);
	//
	asset* ainsley = new asset;
	ainsley->texture = ainsleyTexture;
	ainsley->identity = "ainsley";
	assets.insert(assets.begin(), pair<string, asset>("ainsley", *ainsley));

	//

	sf::Texture heartTexture;
	heartTexture.loadFromFile("./dependencies/assets/heart.png", sf::IntRect(0, 0, 32, 32));
	heartTexture.setSmooth(true);
	//
	asset* heart = new asset;
	heart->texture = heartTexture;
	heart->identity = "heart";
	assets.insert(assets.begin(), pair<string, asset>("heart", *heart));

	//

	sf::Texture bulletTexture;
	bulletTexture.loadFromFile("./dependencies/assets/bullets.png", sf::IntRect(0, 0, 8, 8));
	bulletTexture.setSmooth(true);
	//
	asset* bullet = new asset;
	bullet->texture = bulletTexture;
	bullet->identity = "bullet";
	assets.insert(assets.begin(), pair<string, asset>("bullet", *bullet));

	return assets;

}

void generateGround() {
	for (int i = 1; i != 1042; i++) {
		int randomInt = rand() % 2 + 1;
		if (randomInt == 1) {
			// Blank Grass
			tiles[i].identity = "blank grass";
			tiles[i].location = i;

			groundTiles[i].identity = "blank grass";
			groundTiles[i].location = i;
		}
		else if (randomInt == 2) {
			// Leaf Grass
			tiles[i].identity = "leaf grass";
			tiles[i].location = i;

			groundTiles[i].identity = "leaf grass";
			groundTiles[i].location = i;
		}
	}
}

void generateAinsley() {
	for (int i = 0; i != 15; i++) {
		int randomInt = rand() % 1000 + 40;
		tiles[randomInt].identity = "ainsley";
	}
}

void generateHeart() {
	for (int i = 0; i != 5; i++) {
		int randomInt = rand() % 1041 + 1;
		tiles[randomInt].identity = "heart";
	}
}

// Returns true if the unsigned x is in between the two low and high numbers.
bool inRange(unsigned low, unsigned high, unsigned x)
{
	return  ((x - low) <= (high - low));
}

void changeTile(int tileNum, std::string newIdentity) {
	tiles[tileNum].identity = newIdentity;
}

void updateTiles(sf::RenderWindow& window, std::map<string, asset> assets, std::vector<tile> tiles) {
	// Sprite Creation, takes textures from the assets map and creates a sprite with it.

	sf::Sprite character;
	character.setTexture(assets.at("character").texture);

	sf::Sprite ground1;
	ground1.setTexture(assets.at("blank grass").texture);

	sf::Sprite ground2;
	ground2.setTexture(assets.at("leaf grass").texture);

	sf::Sprite ainsley;
	ainsley.setTexture(assets.at("ainsley").texture);

	sf::Sprite heart;
	heart.setTexture(assets.at("heart").texture);

	int x = 0, y = 0;

	for (int i = 1; i != 1042; i++) {

		// Create a sprite for the current ground (For use with hearts and other transparent backgrounds)
		sf::Sprite currGround;
		currGround.setTexture(assets.at(groundTiles[i].identity).texture);

		tiles[i].vecLocation.x = x;
		tiles[i].vecLocation.y = y;

		if (tiles[i].identity == "blank grass") {
			ground1.setPosition(x, y);
			window.draw(ground1);
		}
		else if (tiles[i].identity == "leaf grass") {
			ground2.setPosition(x, y);
			window.draw(ground2);
		}
		else if (tiles[i].identity == "character") {
			if (characterDead == false) {
				if (invertCharacter == true) {

					character.setScale(-1, 1); // Flip (invert char because they shot left)
					character.setPosition(x + 32, y);
				}
				else {

					character.setScale(1, 1); 
					character.setPosition(x, y);
				}
				window.draw(character);
				charPos = sf::Vector2f(x, y);
			}
		}
		else if (tiles[i].identity == "ainsley") {
			ainsley.setPosition(x, y);
			window.draw(ainsley);
			// Check if bullet is in range of tiles x and x + 32 and matches y pos of bullet
			if (inRange(x, x + 32, calcPos.x) == true && calcPos.y - 7 == y) {
				changeTile(i, groundTiles[i].identity);
				cout << "hit" << endl;
				bulletSpawned = false;
			}
		}
		else if (tiles[i].identity == "heart") {
			currGround.setPosition(x, y);
			window.draw(currGround);
			heart.setPosition(x, y);
			window.draw(heart);
		}



		x = x + 32;
		if (i % 40 == 0) {
			x = 0;
			y = y + 32;
		}
	}
}


void updateHP(sf::RenderWindow& window, sf::Text HP, int health) {
	HP.setString(to_string(health));
	if (health < 1) {
		tiles[charTile].identity = groundTiles[charTile].identity;
		characterDead = true;
		
		HP.setString("DEAD.");
	}
	window.draw(HP);
}

void bulletHandler(sf::RenderWindow& window, std::map<string, asset> assets, sf::Time elapsedTime, bullet Bullet) {
	// Regular bullet
	sf::Sprite regBullet;
	regBullet.setTexture(assets.at("bullet").texture);
	bulletSpeed = bulletSpeedConst * elapsedTime.asSeconds();

	// Main bullet check loop

	if (bulletSpawned = true) {
		if (Bullet.direction == "right") {
			calcPos = Bullet.location;
			calcPos.x = Bullet.location.x + bulletSpeed;
			Bullet.location = calcPos;

			regBullet.setPosition(calcPos);

		}
		else if (Bullet.direction == "left") {
			calcPos = Bullet.location;
			calcPos.x = Bullet.location.x - bulletSpeed;
			Bullet.location = calcPos;

			regBullet.setPosition(calcPos);

		}
		if (Bullet.location.x > Bullet.startLocation.x + 500 || Bullet.location.x < Bullet.startLocation.x - 500 || Bullet.location.x >= 1270 || Bullet.location.x <= 10) {
			bulletSpeed = 0;
			Bullet.location = sf::Vector2f(0, 0);
			calcPos = sf::Vector2f(0, 0);
			bulletSpawned = false;
		}
		window.draw(regBullet);
	}
}


void moveTiles(sf::RenderWindow& window, std::map<string, asset> assets, std::vector<tile> tiles) {

	for (int i = 1; i != 1042; i++) {
		if (tiles[i].identity == "ainsley") {
			lastMove = lastMove + 1;
			cout << lastMove << endl;
			int randInt = rand() % 3 + 1;
			if (lastMove >= ainsleySpeed) {
				lastMove = 0;
				switch (randInt) {
				case 1:
					// Move ainsley up
					newPos = tiles[i].location - 40;
					if (newPos < 40) {
						newPos = tiles[i].location - 40;
					}
					else if (newPos > 1000) {
						newPos = tiles[i].location + 40;
					}
					else {
						changeTile(tiles[i].location, groundTiles[i].identity);
						changeTile(newPos, "ainsley");
					}
					break;
				case 2:
					// Move ainsley down
					newPos = tiles[i].location + 40;
					if (newPos < 40) {
						newPos = tiles[i].location - 40;
					}
					else if (newPos > 1000) {
						newPos = tiles[i].location + 40;
					}
					else {
						changeTile(tiles[i].location, groundTiles[i].identity);
						changeTile(newPos, "ainsley");
					}
					break;
				case 3:
					// Move ainsley right
					newPos = tiles[i].location + 1;
					if (newPos < 40) {
						newPos = tiles[i].location - 40;
					}
					else if (newPos > 1000) {
						newPos = tiles[i].location + 40;
					}
					else {
						changeTile(tiles[i].location, groundTiles[i].identity);
						changeTile(newPos, "ainsley");
					}
					break;
				case 4:
					// Move ainsley left
					newPos = tiles[i].location - 1;
					if (newPos < 40) {
						newPos = tiles[i].location - 40;
					}
					else if (newPos > 1000) {
						newPos = tiles[i].location + 40;
					}
					else {
						changeTile(tiles[i].location, groundTiles[i].identity);
						changeTile(newPos, "ainsley");
					}
					break;
				}
			}
		}
	}
}

/*
Main
*/

int main() {
	// Rand seed
	srand(time(NULL));

	/*
	SFML Setup
	-- Make main window
	-- call LoadAssets() function and assign the return map to "assets"
	-- Generate the ground
	-- Generate the Ainsley's
	-- Generate the hearts
	-- Create "HP" text to display heart count
	*/

	sf::RenderWindow window(sf::VideoMode(1280, 832), "Attack of the Ainsley's"); // Create main window
	std::map<string, asset> assets = loadAssets();

	sf::Font bitFont;
	bitFont.loadFromFile("./dependencies/assets/8bit.ttf");

	// HP

	sf::Text HP;
	HP.setFont(bitFont);
	HP.setCharacterSize(32);
	HP.setFillColor(sf::Color::Red);

	// Music

	sf::SoundBuffer sandstormBuffer;
	sandstormBuffer.loadFromFile("./dependencies/assets/sandstorm.ogg");
	sf::Sound sandstorm;
	sandstorm.setBuffer(sandstormBuffer);
	
	// Gun sound

	sf::SoundBuffer gunBuffer;
	gunBuffer.loadFromFile("./dependencies/assets/laser1.wav");
	sf::Sound gunSound;
	gunSound.setBuffer(gunBuffer);

	// Bullet

	bullet newBullet;

	// Main Menu Objects

	sf::RectangleShape Rect;
	Rect.setPosition(0,0);
	Rect.setSize(sf::Vector2f(window.getSize().x, window.getSize().y));
	Rect.setFillColor(sf::Color(0, 0, 128));


	sf::Text title;
	title.setFont(bitFont);
	title.setCharacterSize(32);
	title.setFillColor(sf::Color::Red);


	// Generate the ground

	generateGround();
	generateAinsley();
	generateHeart();

	tiles[charTile].identity = "character";

	sf::Clock Clock; // Clock obj for elapsed time

	sandstorm.setLoop(true);
	sandstorm.setVolume(75);
	sandstorm.play();


	while (window.isOpen()) {


		sf::Event event;
		while (window.pollEvent(event))
		{
			// Close window: exit
			if (event.type == sf::Event::Closed)
				window.close();
		}


			sf::Time ElapsedTime = Clock.getElapsedTime();
			sf::Time res = Clock.restart();


			if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) && characterDead == false)
			{
				int oldPos = charTile;
				charTile = charTile - 40;

				if (tiles[charTile].identity == "heart") {
					health++;
				}
				if (charTile < 40) {
					charTile = oldPos;
					tiles[oldPos].identity = "character";
				}
				else {
					tiles[oldPos].identity = groundTiles[oldPos].identity;
					tiles[charTile].identity = "character";
				}
				cout << "moved" << endl;
				sleep_for(milliseconds(100));
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) && characterDead == false)
			{
				int oldPos = charTile;
				charTile = charTile + 40;

				if (tiles[charTile].identity == "heart") {
					health++;
				}

				if (charTile > 1000) {
					charTile = oldPos;
					tiles[oldPos].identity = "character";
				}
				else {
					tiles[oldPos].identity = groundTiles[oldPos].identity;
					tiles[charTile].identity = "character";
				}
				cout << "moved" << endl;
				sleep_for(milliseconds(100));
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) && characterDead == false)
			{
				int oldPos = charTile;
				charTile = charTile - 1;

				if (tiles[charTile].identity == "heart") {
					health++;
				}
				tiles[oldPos].identity = groundTiles[oldPos].identity;
				tiles[charTile].identity = "character";
				cout << "moved" << endl;
				sleep_for(milliseconds(100));
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) && characterDead == false)
			{
				int oldPos = charTile;
				charTile = charTile + 1;

				if (tiles[charTile].identity == "heart") {
					health++;
				}
				tiles[oldPos].identity = groundTiles[oldPos].identity;
				tiles[charTile].identity = "character";
				cout << "moved" << endl;
				sleep_for(milliseconds(100));
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q) && characterDead == false)
			{
				invertCharacter = true;
				if (bulletSpawned == false) {
					newBullet.direction = "left";
					newBullet.identity = "regular";
					newBullet.location.y = charPos.y + 7;
					newBullet.location.x = charPos.x - 0;
					newBullet.startLocation = newBullet.location;
					bulletSpawned = true;
					gunSound.play();
				}
				sleep_for(milliseconds(100));
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::E) && characterDead == false)
			{
				invertCharacter = false;
				if (bulletSpawned == false) {
					newBullet.direction = "right";
					newBullet.identity = "regular";
					newBullet.location.y = charPos.y + 7;
					newBullet.location.x = charPos.x - 0;
					newBullet.startLocation = newBullet.location;
					bulletSpawned = true;
					gunSound.play();
				}
				sleep_for(milliseconds(100));
			}



			window.clear(sf::Color::Black);


			updateTiles(window, assets, tiles);
			updateHP(window, HP, health);
			moveTiles(window, assets, tiles);
			bulletHandler(window, assets, ElapsedTime, newBullet);
			// Reset shoot values
			shootLeft = false;
			shootRight = false;
			newBullet.location = calcPos;

			window.display();

		}


	return EXIT_SUCCESS;
}