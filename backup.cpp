/*
Dependencies & Libraries
*/

#include <SFML\Graphics.hpp>
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
};


/*
Variables
*/

bool debugMode = true;
int charTile = 250;
bool characterDead = false;
bool invertCharacter = false;
bool shotRight = false;
bool shotLeft = false;
sf::Vector2f bulletPos = {0,0};
sf::Vector2f shotPos = { 0,0 };

std::vector<tile> tiles(1042);
std::vector<tile> groundTiles(1042);

float bulletPos;

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
		int randomInt = rand() % 1041 + 1;
		tiles[randomInt].identity = "ainsley";
	}
}

void generateHeart() {
	for (int i = 0; i != 5; i++) {
		int randomInt = rand() % 1041 + 1;
		tiles[randomInt].identity = "heart";
	}
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

	sf::Sprite bullet;
	bullet.setTexture(assets.at("bullet").texture);

	int x = 0, y = 0;

	for (int i = 1; i != 1042; i++) {

		// Create a sprite for the current ground (For use with hearts and other transparent backgrounds)
		sf::Sprite currGround;
		currGround.setTexture(assets.at(groundTiles[i].identity).texture);

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

					character.setScale(-1, 1); // Flip (inverse char because they shot left)
					character.setPosition(x + 32, y);
				}
				else {

					character.setScale(1, 1); 
					character.setPosition(x, y);
				}
				window.draw(character);
			}
			else {
				tiles[i].identity = groundTiles[i].identity;
			}
		}
		else if (tiles[i].identity == "ainsley") {
			ainsley.setPosition(x, y);
			window.draw(ainsley);
		}
		else if (tiles[i].identity == "heart") {
			currGround.setPosition(x, y);
			window.draw(currGround);
			heart.setPosition(x, y);
			window.draw(heart);
		}


		if (shotLeft == true) {

			shotLeft = false;
		}
		if (shotRight == true) {



			shotRight = false;
		}

		x = x + 32;
		if (i % 40 == 0) {
			x = 0;
			y = y + 32;
		}
	}

}

void shootLeft() {
	int shootLocation = charTile - 2;
	shotLeft = true;

	if (tiles[shootLocation].identity == "ainsley") {
		tiles[shootLocation].identity = groundTiles[shootLocation].identity;
	}
	else {
		health = health - 1;
	}
}

void shootRight() {
	int shootLocation = charTile + 2;
	shotRight = true;

	if (tiles[shootLocation].identity == "ainsley") {
		tiles[shootLocation].identity = groundTiles[shootLocation].identity;
	}
	else {
		health = health - 1;
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

void updateBullet(sf::RenderWindow& window, sf::Vector2f bulletPos) {



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

	// HP

	sf::Font font;
	font.loadFromFile("./dependencies/assets/arial.ttf");
	sf::Text HP;
	HP.setFont(font);
	HP.setCharacterSize(32);
	HP.setFillColor(sf::Color::Red);



	// Generate the ground

	generateGround();
	generateAinsley();
	generateHeart();

	tiles[charTile].identity = "character";

	sf::Clock Clock;

	const float Speed = 50.f;
	float Left = 0.f;
	float Top = 0.f;

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

			tiles[oldPos].identity = groundTiles[oldPos].identity;
			tiles[charTile].identity = "character";
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

			tiles[oldPos].identity = groundTiles[oldPos].identity;
			tiles[charTile].identity = "character";
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

			shootLeft = true;
			sleep_for(milliseconds(100));
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::E) && characterDead == false)
		{
			invertCharacter = false;

			shootRight = true;
			sleep_for(milliseconds(100));
		}



		window.clear(sf::Color::Black);


		updateTiles(window, assets, tiles);
		updateHP(window, HP, health);
		updateBullet(window, bulletPos);

		window.display();

	}
	return EXIT_SUCCESS;
}
