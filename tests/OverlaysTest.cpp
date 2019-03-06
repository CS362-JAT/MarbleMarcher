//using google test 1.8.1

//include gtest.h
#include "gtest/gtest.h"
//include files to test
#include "../src/Overlays.h"
#include "../src/Res.h"

namespace {

class OverlaysTest : public ::testing::Test {
	protected:
		OverlaysTest()
		{
			//set up work
			font.sf::Font::loadFromFile(Orbitron_Bold_ttf);
			font_mono.sf::Font::loadFromFile(Inconsolata_Bold_ttf);

			overlays = new Overlays(&font, &font_mono);
			/*
			settings.majorVersion = 2;
			settings.minorVersion = 0;

			screen_size = sf::VideoMode::getDesktopMode();

			window_style = sf::Style::Fullscreen;

			window = RenderWindow(screen_size, "Marble Marcher", window_style, settings);
			window.setVerticalSyncEnabled(true);
			window.setKeyRepeatEnabled(false);
			window.requestFocus();
			*/
		}

		~OverlaysTest() override
		{
			//clean up work
		}

		void SetUp() override 
		{
			//called before each test after constructor
		}

		void TearDown() override
		{
			//called after each test before destructor
		}

		//objects declared here can be used by all tests
		Overlays *overlays;
		//sf::RenderWindow window;
		//sf::VideoMode screen_size;
		//sf::Uint32 window_style;
		//sf::ContextSettings settings;
		sf::Font font;
		sf::Font font_mono;
		float mouse_x;
		float mouse_y;
};

TEST_F(OverlaysTest, Menu_NoHover)
{
	mouse_x = 0.0;
	mouse_y = 0.0;
	
	overlays->UpdateMenu(mouse_x, mouse_y);

	for(int i = 1; i <= 5; i++)
	{
		EXPECT_FALSE(overlays->getAllHover()[i]);
	}
}

TEST_F(OverlaysTest, Menu_Hover)
{
	mouse_x = 81.0;
	mouse_y = 231.0;

	overlays->UpdateMenu(mouse_x, mouse_y);

	EXPECT_TRUE(overlays->getAllHover()[1]);
}

TEST_F(OverlaysTest, Controls_Hover)
{
	mouse_x = 61.0;
	mouse_y = 551.0;

	overlays->UpdateControls(mouse_x, mouse_y);

	EXPECT_TRUE(overlays->getAllHover()[15]);
}

TEST_F(OverlaysTest, Levels_Hover)
{
	mouse_x = 641.0;
	mouse_y = 201.0;

	overlays->UpdateLevels(mouse_x, mouse_y);

	EXPECT_TRUE(overlays->getAllHover()[17]);
}

TEST_F(OverlaysTest, Paused_Hover)
{
	mouse_x = 371.0;
	mouse_y = 357.0;

	overlays->UpdatePaused(mouse_x, mouse_y);

	EXPECT_TRUE(overlays->getAllHover()[8]);
}

} //namespace

int main(int argc, char **argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
