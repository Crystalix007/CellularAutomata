#define SFML_STATIC

#include <SFML/Graphics.hpp>
#include <random>
#include "AutomatonType.h"

using namespace std;

const size_t initialLife = 50, lifeAddition = 10, birthReq = 150, birthCost = 2 * initialLife, overPopulation = 220;
const size_t screenWidth = 800, screenHeight = 640;

size_t getRand();
AutomatonType getRandAutomatonType();
AutomatonType getAutomatonType(sf::Color &&pixelColour);
void setupBoard(sf::Image& img);
void updateBoard(sf::Image &img);

int main()
{
	sf::RenderWindow window{ sf::VideoMode(screenWidth, screenHeight), "Cellular Automata!" };
	window.setFramerateLimit(60);
	window.setVerticalSyncEnabled(true);

	sf::Image image{};
	setupBoard(image);

	sf::Texture texture{};
	texture.loadFromImage(image);

	sf::Sprite sprite{ texture };

	while (window.isOpen())
	{
		sf::Event event{};

		while (window.pollEvent(event))
		{
			if (event.type == event.Closed)
				window.close();
			if (event.type == event.KeyPressed && event.key.code == sf::Keyboard::Escape)
				window.close();
		}

		updateBoard(image);
		texture.loadFromImage(image);
		//sprite.setTexture(texture);
		window.draw(sprite);
		window.display();
	}
}

size_t getRand()
{
	static random_device rd{};
	static uniform_int<size_t> distribution{ 0, 1000 };
	static mt19937 rnd{ rd() };
	return distribution(rnd);
}

AutomatonType getRandAutomatonType()
{
	//Weightings empirically determined
	auto randNum = getRand();

	const size_t carnivoreWeighting = 50;
	const size_t herbivoreWeighting = 400;

	if (randNum < carnivoreWeighting)
		return AutomatonType::Carnivore;
	randNum -= carnivoreWeighting;

	if (randNum < herbivoreWeighting)
		return AutomatonType::Herbivore;
	randNum -= herbivoreWeighting;

	return AutomatonType::Dead;
}

AutomatonType getAutomatonType(sf::Color&& pixelColour)
{
	if (pixelColour.r > 0)
		return AutomatonType::Carnivore;
	else if (pixelColour.g > 0)
		return AutomatonType::Herbivore;
	else
		return AutomatonType::Dead;
}

void setupBoard(sf::Image& img)
{
	img.create(screenWidth, screenHeight);

	for (size_t x = 0; x < img.getSize().x; ++x)
	{
		for (size_t y = 0; y < img.getSize().y; ++y)
		{
			sf::Color pixCol{ 0, 0, 0 };

			switch (getRandAutomatonType())
			{
			default:
			case AutomatonType::Dead:
				break;
			case AutomatonType::Herbivore:
				pixCol.g = initialLife;
				break;
			case AutomatonType::Carnivore:
				pixCol.r = initialLife;
				break;
			}

			img.setPixel(x, y, pixCol);
		}
	}
}

void updateBoard(sf::Image& img)
{
	const auto imageSize = img.getSize();
	const auto imageWidth = imageSize.x, imageHeight = imageSize.y;

	for (size_t x = 0; x < imageWidth; ++x)
	{
		for (size_t y = 0; y < imageHeight; ++y)
		{
			const auto pixCol = img.getPixel(x, y);

			if (pixCol.r > 0) //Carnivore
			{
				if (pixCol.r >= birthReq) //Spawn a new carnivore
				{
					if (x > 0 && getAutomatonType(img.getPixel(x - 1, y)) == AutomatonType::Dead)
					{
						img.setPixel(x - 1, y, sf::Color(initialLife, 0, 0));
						auto newPixCol = pixCol;
						newPixCol.r -= birthCost;
						img.setPixel(x, y, newPixCol);
					}
					else if (y > 0 && getAutomatonType(img.getPixel(x, y - 1)) == AutomatonType::Dead)
					{
						img.setPixel(x, y - 1, sf::Color(initialLife, 0, 0));
						auto newPixCol = pixCol;
						newPixCol.r -= birthCost;
						img.setPixel(x, y, newPixCol);
					}
					else if (x + 1 < imageWidth && getAutomatonType(img.getPixel(x + 1, y)) == AutomatonType::Dead)
					{
						img.setPixel(x + 1, y, sf::Color(initialLife, 0, 0));
						auto newPixCol = pixCol;
						newPixCol.r -= birthCost;
						img.setPixel(x, y, newPixCol);
					}
					else if (y + 1 < imageWidth && getAutomatonType(img.getPixel(x, y + 1)) == AutomatonType::Dead)
					{
						img.setPixel(x, y + 1, sf::Color(initialLife, 0, 0));
						auto newPixCol = pixCol;
						newPixCol.r -= birthCost;
						img.setPixel(x, y, newPixCol);
					}
					else if (pixCol.r >= overPopulation)  //Over-population
						img.setPixel(x, y, pixCol.Black);
				}

				if (x > 0 && getAutomatonType(img.getPixel(x - 1, y)) == AutomatonType::Herbivore)
				{
					img.setPixel(x - 1, y, pixCol.Black);
					auto newPixCol = pixCol;
					newPixCol.r += lifeAddition;
					img.setPixel(x, y, newPixCol);
				}
				else if (y > 0 && getAutomatonType(img.getPixel(x, y - 1)) == AutomatonType::Herbivore)
				{
					img.setPixel(x, y - 1, pixCol.Black);
					auto newPixCol = pixCol;
					newPixCol.r += lifeAddition;
					img.setPixel(x, y, newPixCol);
				}
				else if (x + 1 < imageWidth && getAutomatonType(img.getPixel(x + 1, y)) == AutomatonType::Herbivore)
				{
					img.setPixel(x + 1, y, pixCol.Black);
					auto newPixCol = pixCol;
					newPixCol.r += lifeAddition;
					img.setPixel(x, y, newPixCol);
				}
				else if (y + 1 < imageWidth && getAutomatonType(img.getPixel(x, y + 1)) == AutomatonType::Herbivore)
				{
					img.setPixel(x, y + 1, pixCol.Black);
					auto newPixCol = pixCol;
					newPixCol.r += lifeAddition;
					img.setPixel(x, y, newPixCol);
				}
				else //Starvation
				{
					auto newPixCol = pixCol;
					newPixCol.r -= lifeAddition;
					img.setPixel(x, y, newPixCol);
				}
			}
			else if (pixCol.g > 0) //Herbivore
			{
				if (pixCol.g >= birthReq) //Spawn a new herbivore
				{
					if (x > 0 && getAutomatonType(img.getPixel(x - 1, y)) == AutomatonType::Dead)
					{
						img.setPixel(x - 1, y, sf::Color(0, initialLife, 0));
						auto newPixCol = pixCol;
						newPixCol.g -= birthCost;
						img.setPixel(x, y, newPixCol);
					}
					else if (y > 0 && getAutomatonType(img.getPixel(x, y - 1)) == AutomatonType::Dead)
					{
						img.setPixel(x, y - 1, sf::Color(0, initialLife, 0));
						auto newPixCol = pixCol;
						newPixCol.g -= birthCost;
						img.setPixel(x, y, newPixCol);
					}
					else if (x + 1 < imageWidth && getAutomatonType(img.getPixel(x + 1, y)) == AutomatonType::Dead)
					{
						img.setPixel(x + 1, y, sf::Color(0, initialLife, 0));
						auto newPixCol = pixCol;
						newPixCol.g -= birthCost;
						img.setPixel(x, y, newPixCol);
					}
					else if (y + 1 < imageWidth && getAutomatonType(img.getPixel(x, y + 1)) == AutomatonType::Dead)
					{
						img.setPixel(x, y + 1, sf::Color(0, initialLife, 0));
						auto newPixCol = pixCol;
						newPixCol.r -= birthCost;
						img.setPixel(x, y, newPixCol);
					}
					else if (pixCol.g > overPopulation)//Over-population
						img.setPixel(x, y, pixCol.Black);
				}

				auto newPixCol = pixCol;
				newPixCol.g += lifeAddition;
				img.setPixel(x, y, newPixCol);
			}
		}
	}
}
