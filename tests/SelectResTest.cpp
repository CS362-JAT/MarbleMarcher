#include "pch.h"
#include "Scene.h"
#include "Scene.cpp"
#include "Level.h"
#include "Level.cpp"
#include "Overlays.h"
#include "Res.h"
#include "Scores.h"
#include "Scores.cpp"
#include "SelectRes.h"

TEST(SelectResolution, OutOfBounds) {
	sf::Font font;
	font.loadFromFile("../assets/Orbitron-Bold.ttf");
	SelectRes selectedRes(&font);

	sf::Vector2i pos1(30, 10000);
	sf::Vector2i pos1(30, -10);

	EXPECT_EQ(selectedRes.Select(pos1), -1);
	EXPECT_EQ(selectedRes.Select(pos2), -1);
}

TEST(SelectResolution, InBoundsSelected) {
	sf::Font font;
	font.loadFromFile("../assets/Orbitron-Bold.ttf");
	SelectRes selectedRes(&font);

	sf::Vector2i pos1(30, 120);
	sf::Vector2i pos2(30, 190);
	sf::Vector2i pos3(30, 250);
	sf::Vector2i pos4(30, 310);
	sf::Vector2i pos5(30, 380);
	sf::Vector2i pos6(30, 440);
	sf::Vector2i pos7(30, 500);

	EXPECT_EQ(selectedRes.Select(pos1), 0);
	EXPECT_EQ(selectedRes.Select(pos2), 1);
	EXPECT_EQ(selectedRes.Select(pos3), 2);
	EXPECT_EQ(selectedRes.Select(pos4), 3);
	EXPECT_EQ(selectedRes.Select(pos5), 4);
	EXPECT_EQ(selectedRes.Select(pos6), 5);
	EXPECT_EQ(selectedRes.Select(pos7), 6);
}

TEST(SelectResolution, InBoundsNotSelected) {
	sf::Font font;
	font.loadFromFile("../assets/Orbitron-Bold.ttf");
	SelectRes selectedRes(&font);

	sf::Vector2i pos1(30, 465);

	EXPECT_EQ(selectedRes.Select(pos1), -1);
}


TEST(MakeText, TextCreationNonCentered) {
	sf::Font font;
	font.loadFromFile("../assets/Orbitron-Bold.ttf");
	SelectRes selectedRes(&font);

	char* str = "hello world";

	sf::Text text1 = selectedRes.MakeText(str, 25f, 25f, false, false);

	sf::Vector2f pos = text.getPosition();

	EXPECT_EQ(pos.x, 25f);
	EXPECT_EQ(pos.y, 25f);

	sf::Vector2f origin = text.getOrigin();

	EXPECT_EQ(pos.x, 0);
	EXPECT_EQ(pos.y, 0);
}

TEST(MakeText, TextCreationCentered) {
	sf::Font font;
	font.loadFromFile("../assets/Orbitron-Bold.ttf");
	SelectRes selectedRes(&font);

	char* str = "hello world";

	sf::Text text1 = selectedRes.MakeText(str, 25f, 25f, 12, false, true);

	EXPECT_EQ(text1.getSize(), 12);

	sf::Vector2f pos = text1.getPosition();

	EXPECT_EQ(pos.x, 25f);
	EXPECT_EQ(pos.y, 25f);

	sf::Vector2f origin = text1.getOrigin();

	EXPECT_NE(origin.x, 0);
	EXPECT_NE(origin.y, 0);
}

TEST(MakeText, TextCreationCentered) {
	sf::Font font;
	font.loadFromFile("../assets/Orbitron-Bold.ttf");
	SelectRes selectedRes(&font);

	char* str = "hello world";

	sf::Text text1 = selectedRes.MakeText(str, 25f, 25f, 12, false, false);

	EXPECT_EQ(text1.getSize(), 12);

	sf::Vector2f pos = text1.getPosition();

	EXPECT_EQ(pos.x, 25f);
	EXPECT_EQ(pos.y, 25f);

	sf::Vector2f origin = text1.getOrigin();

	EXPECT_EQ(origin.x, 0);
	EXPECT_EQ(origin.y, 0);
}

TEST(MakeText, TextCreationSelected) {
	sf::Font font;
	font.loadFromFile("../assets/Orbitron-Bold.ttf");
	SelectRes selectedRes(&font);

	char* str = "hello world";

	sf::Text text1 = selectedRes.MakeText(str, 25f, 25f, 12, true, false);

	EXPECT_EQ(text1.getSize(), 12);

	sf::Vector2f pos = text1.getPosition();

	EXPECT_EQ(pos.x, 25f);
	EXPECT_EQ(pos.y, 25f);

	sf::Color color = text1.getColor();

	EXPECT_EQ(color.r, 255);
	EXPECT_EQ(color.g, 255);
	EXPECT_EQ(color.b, 255);
}

TEST(MakeText, TextCreationNotSelected) {
	sf::Font font;
	font.loadFromFile("../assets/Orbitron-Bold.ttf");
	SelectRes selectedRes(&font);

	char* str = "hello world";

	sf::Text text1 = selectedRes.MakeText(str, 25f, 25f, 12, false, false);

	EXPECT_EQ(text1.getSize(), 12);

	sf::Vector2f pos = text1.getPosition();

	EXPECT_EQ(pos.x, 25f);
	EXPECT_EQ(pos.y, 25f);

	sf::Color color = text1.getColor();

	EXPECT_EQ(color.r, 122);
	EXPECT_EQ(color.g, 122);
	EXPECT_EQ(color.b, 122);
}

