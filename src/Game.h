#include "Scene.h"
#include "Overlays.h"
#include "Res.h"
#include "SelectRes.h"
#include "Scores.h"
#include "Level.h"

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>

#include <sys/types.h>
#include <sys/stat.h>

#include <string>
#include <iostream>
#include <fstream>
#include <thread>
#include <mutex>

#ifdef _WIN32
#include <Windows.h>
#define ERROR_MSG(x) MessageBox(nullptr, TEXT(x), TEXT("ERROR"), MB_OK);
#else
#define ERROR_MSG(x) std::cerr << x << std::endl;
#endif

static const float mouse_sensitivity = 0.005f;
static const float wheel_sensitivity = 0.2f;
static const float music_vol = 75.0f;
static const float target_fps = 60.0f;

class Game {
public:
	//Game modes
	enum GameMode {
	  MAIN_MENU,
	  PLAYING,
	  PAUSED,
	  SCREEN_SAVER,
	  CONTROLS,
	  LEVELS,
	  CREDITS
	};

	Game();

	float GetVol();
	int CheckEnvironment();
	void LockMouse(sf::RenderWindow& window);
	void UnlockMouse(sf::RenderWindow& window);
	int dirExists(const char *path);

	void LoadMusic();
	int GetDirectory();
	int SetResolution();
	void CreateWindow();
	void CreateRenderTexture();
	void CreateFractalScene();
	void CreateMenus();
	void GameLoop();
private:
	sf::Shader shader;
	sf::Font font;
	sf::Font font_mono;

	sf::Vector2i mouse_pos;
	bool all_keys[sf::Keyboard::KeyCount];
	bool mouse_clicked;
	bool show_cheats;
	GameMode game_mode;

	sf::Music menu_music;
	sf::Music level1_music;
	sf::Music level2_music;
	sf::Music credits_music;

  	std::string save_dir;
	bool success;
  	std::string save_file;

	Resolution* resolution;
	bool fullscreen;

	sf::ContextSettings settings;

	sf::RenderWindow* window;
	sf::VideoMode screen_size;
	sf::Uint32 window_style;
	sf::Vector2i* screen_center;

	sf::RenderTexture renderTexture;
	
	Scene* scene;
	sf::Glsl::Vec2* window_res;
	Overlays* overlays;
};