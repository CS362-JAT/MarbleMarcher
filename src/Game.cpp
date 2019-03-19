#include "Game.h"
#include "Scene.h"
#include "Scores.h"
#include "Overlays.h"

#include <stdlib.h>

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>

Game::Game(){
	all_keys[sf::Keyboard::KeyCount] = {0};
	mouse_clicked = false;
	show_cheats = false;
	show_cheats = false;
	GameMode game_mode = MAIN_MENU;

	settings.majorVersion = 2;
	settings.minorVersion = 0;

	CheckEnvironment();

	LoadMusic();

	GetDirectory();
	SetResolution();

	CreateWindow();

	CreateRenderTexture();

	scene = new Scene(&level1_music, &level2_music);
	window_res = new sf::Glsl::Vec2((float)resolution->width, (float)resolution->height);
	shader.setUniform("iResolution", window_res);
	scene->Write(shader);

	//Create the menus
	overlays = new Overlays(&font, &font_mono);
	  overlays->SetScale(float(screen_size.width) / 1280.0f);
	  menu_music.setVolume(GetVol());
	  menu_music.play(); 
}

int Game::CheckEnvironment(){
	//Make sure shader is supported
	if (!sf::Shader::isAvailable()) {
		ERROR_MSG("Graphics card does not support shaders");
		exit(EXIT_FAILURE);
	}
	//Load the vertex shader
	if (!shader.loadFromFile(vert_glsl, sf::Shader::Vertex)) {
		ERROR_MSG("Failed to compile vertex shader");
		exit(EXIT_FAILURE);
	}
	//Load the fragment shader
	if (!shader.loadFromFile(frag_glsl, sf::Shader::Fragment)) {
		ERROR_MSG("Failed to compile fragment shader");
		exit(EXIT_FAILURE);
	}

	//Load the font
	if (!font.loadFromFile(Orbitron_Bold_ttf)) {
		ERROR_MSG("Unable to load font");
		exit(EXIT_FAILURE);
	}
	//Load the mono font
	if (!font_mono.loadFromFile(Inconsolata_Bold_ttf)) {
		ERROR_MSG("Unable to load mono font");
		exit(EXIT_FAILURE);
	}
	return 0;
}

void Game::LoadMusic(){
	//Load the music
	menu_music.openFromFile(menu_ogg);
	menu_music.setLoop(true);
	level1_music.openFromFile(level1_ogg);
	level1_music.setLoop(true);
	level2_music.openFromFile(level2_ogg);
	level2_music.setLoop(true);
	credits_music.openFromFile(credits_ogg);
	credits_music.setLoop(true);
}

int Game::GetDirectory(){
	//Get the directory for saving and loading high scores
	#ifdef _WIN32
	  save_dir = std::string(std::getenv("APPDATA")) + "\\MarbleMarcher";
	#else
	  save_dir = std::string(std::getenv("HOME")) + "/.MarbleMarcher";
	#endif
	  
	  if (!dirExists(save_dir.c_str())) {
	#if defined(_WIN32)
	    success = CreateDirectory(save_dir.c_str(), NULL) != 0 || GetLastError() == ERROR_ALREADY_EXISTS;
	#else
	    success = mkdir(save_dir.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == 0;
	#endif
	    if (!success) {
	      ERROR_MSG("Failed to create save directory");
	      return 1;
	    }
	  }
	  save_file = save_dir + "/scores.bin";

	//Load scores if available
	  high_scores.Load(save_file);
	return 0;
}

int Game::SetResolution(){
	//Have user select the resolution
	SelectRes select_res(&font_mono);
	resolution = select_res.Run();
	fullscreen = select_res.FullScreen();
	if (resolution == nullptr) {
		return 0;
	}
	return 1;
}

void Game::CreateWindow(){
	//Create the window
	screen_center = new sf::Vector2i(resolution->width / 2, resolution->height / 2);
	if (fullscreen) {
		screen_size = sf::VideoMode::getDesktopMode();
		window_style = sf::Style::Fullscreen;
	} else {
		screen_size = sf::VideoMode(resolution->width, resolution->height, 24);
		window_style = sf::Style::Close;
	}

	window = new sf::RenderWindow(screen_size, "Marble Marcher", window_style, settings);
	window->setVerticalSyncEnabled(true);
	window->setKeyRepeatEnabled(false);
	window->requestFocus();
}

void Game::CreateRenderTexture(){
	  //If native resolution is the same, then we don't need a render texture
	  if (resolution->width == screen_size.width && resolution->height == screen_size.height) {
	    fullscreen = false;
	  }

	  //Create the render texture if needed
	  if (fullscreen) {
	    renderTexture.create(resolution->width, resolution->height, settings);
	    renderTexture.setSmooth(true);
	    renderTexture.setActive(true);
	    window->setActive(false);
	  }
}

void Game::CreateFractalScene(){
	scene = new Scene(&level1_music, &level2_music);
	window_res = new sf::Glsl::Vec2((float)resolution->width, (float)resolution->height);
	shader.setUniform("iResolution", window_res);
	scene->Write(shader);
}

void Game::CreateMenus(){
	//Create the menus
	overlays = new Overlays(&font, &font_mono);
	  overlays->SetScale(float(screen_size.width) / 1280.0f);
	  menu_music.setVolume(GetVol());
	  menu_music.play();
}

void Game::GameLoop(){
	//Main loop
  sf::Clock clock;
  float smooth_fps = 60.0f;
  int lag_ms = 0;
  while (window->isOpen()) {
    sf::Event event;
    float mouse_wheel = 0.0f;
    while (window->pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        window->close();
        break;
      } else if (event.type == sf::Event::KeyPressed) {
        const sf::Keyboard::Key keycode = event.key.code;
        if (event.key.code < 0 || event.key.code >= sf::Keyboard::KeyCount) { continue; }
        if (game_mode == CREDITS) {
          game_mode = MAIN_MENU;
          UnlockMouse(*window);
          scene->SetMode(Camera::INTRO);
          scene->SetExposure(1.0f);
          credits_music.stop();
          menu_music.setVolume(GetVol());
          menu_music.play();
        } else if (keycode == sf::Keyboard::Escape) {
          if (game_mode == MAIN_MENU) {
            window->close();
            break;
          } else if (game_mode == CONTROLS || game_mode == LEVELS) {
            game_mode = MAIN_MENU;
            scene->SetExposure(1.0f);
          } else if (game_mode == SCREEN_SAVER) {
            game_mode = MAIN_MENU;
            scene->SetMode(Camera::INTRO);
          } else if (game_mode == PAUSED) {
            game_mode = PLAYING;
            scene->GetCurMusic().setVolume(GetVol());
            scene->SetExposure(1.0f);
            LockMouse(*window);
          } else if (game_mode == PLAYING) {
            game_mode = PAUSED;
            scene->GetCurMusic().setVolume(GetVol());
            UnlockMouse(*window);
            scene->SetExposure(0.5f);
          }
        } else if (keycode == sf::Keyboard::R) {
          if (game_mode == PLAYING) {
            scene->ResetLevel();
          }
        }
        all_keys[keycode] = true;
      } else if (event.type == sf::Event::KeyReleased) {
        const sf::Keyboard::Key keycode = event.key.code;
        if (event.key.code < 0 || event.key.code >= sf::Keyboard::KeyCount) { continue; }
        all_keys[keycode] = false;
      } else if (event.type == sf::Event::MouseButtonPressed) {
        if (event.mouseButton.button == sf::Mouse::Left) {
          mouse_pos = sf::Vector2i(event.mouseButton.x, event.mouseButton.y);
          if (game_mode == MAIN_MENU) {
            const Overlays::Texts selected = overlays->GetOption(Overlays::PLAY, Overlays::EXIT);
            if (selected == Overlays::PLAY) {
              game_mode = PLAYING;
              menu_music.stop();
              scene->StartNewGame();
              scene->GetCurMusic().setVolume(GetVol());
              scene->GetCurMusic().play();
              LockMouse(*window);
            } else if (selected == Overlays::CONTROLS) {
              game_mode = CONTROLS;
            } else if (selected == Overlays::LEVELS) {
              game_mode = LEVELS;
              scene->SetExposure(0.5f);
            } else if (selected == Overlays::SCREEN_SAVER) {
              game_mode = SCREEN_SAVER;
              scene->SetMode(Camera::SCREEN_SAVER);
            } else if (selected == Overlays::EXIT) {
              window->close();
              break;
            }
          } else if (game_mode == CONTROLS) {
            const Overlays::Texts selected = overlays->GetOption(Overlays::BACK, Overlays::BACK);
            if (selected == Overlays::BACK) {
              game_mode = MAIN_MENU;
            }
          } else if (game_mode == LEVELS) {
            const Overlays::Texts selected = overlays->GetOption(Overlays::L0, Overlays::BACK2);
            if (selected == Overlays::BACK2) {
              game_mode = MAIN_MENU;
              scene->SetExposure(1.0f);
            } else if (selected >= Overlays::L0 && selected <= Overlays::L14) {
              if (high_scores.HasUnlocked(selected - Overlays::L0)) {
                game_mode = PLAYING;
                menu_music.stop();
                scene->SetExposure(1.0f);
                scene->StartSingle(selected - Overlays::L0);
                scene->GetCurMusic().setVolume(GetVol());
                scene->GetCurMusic().play();
                LockMouse(*window);
              }
            }
          } else if (game_mode == SCREEN_SAVER) {
            scene->SetMode(Camera::INTRO);
            game_mode = MAIN_MENU;
          } else if (game_mode == PAUSED) {
            const Overlays::Texts selected = overlays->GetOption(Overlays::CONTINUE, Overlays::MOUSE);
            if (selected == Overlays::CONTINUE) {
              game_mode = PLAYING;
              scene->GetCurMusic().setVolume(GetVol());
              scene->SetExposure(1.0f);
              LockMouse(*window);
            } else if (selected == Overlays::RESTART) {
              game_mode = PLAYING;
              scene->ResetLevel();
              scene->GetCurMusic().setVolume(GetVol());
              scene->SetExposure(1.0f);
              LockMouse(*window);
            } else if (selected == Overlays::QUIT) {
              if (scene->IsSinglePlay()) {
                game_mode = LEVELS;
              } else {
                game_mode = MAIN_MENU;
                scene->SetExposure(1.0f);
              }
              scene->SetMode(Camera::INTRO);
              scene->StopAllMusic();
              menu_music.setVolume(GetVol());
              menu_music.play();
            } else if (selected == Overlays::MUSIC) {
              music_on = !music_on;
              level1_music.setVolume(GetVol());
              level2_music.setVolume(GetVol());
            } else if (selected == Overlays::MOUSE) {
              mouse_setting = (mouse_setting + 1) % 3;
            }
          }
        } else if (event.mouseButton.button == sf::Mouse::Right) {
          if (game_mode == PLAYING) {
            scene->ResetLevel();
          }
        }
      } else if (event.type == sf::Event::MouseButtonReleased) {
        mouse_pos = sf::Vector2i(event.mouseButton.x, event.mouseButton.y);
      } else if (event.type == sf::Event::MouseMoved) {
        mouse_pos = sf::Vector2i(event.mouseMove.x, event.mouseMove.y);
      } else if (event.type == sf::Event::MouseWheelScrolled) {
        mouse_wheel += event.mouseWheelScroll.delta;
      }
    }

    //Check if the game was beat
    if (scene->GetMode() == Camera::FINAL && game_mode != CREDITS) {
      game_mode = CREDITS;
      scene->StopAllMusic();
      scene->SetExposure(0.5f);
      credits_music.play();
    }

    //Main game update
    if (game_mode == MAIN_MENU) {
      scene->UpdateCamera();
      overlays->UpdateMenu((float)mouse_pos.x, (float)mouse_pos.y);
    } else if (game_mode == CONTROLS) {
      scene->UpdateCamera();
      overlays->UpdateControls((float)mouse_pos.x, (float)mouse_pos.y);
    } else if (game_mode == LEVELS) {
      scene->UpdateCamera();
      overlays->UpdateLevels((float)mouse_pos.x, (float)mouse_pos.y);
    } else if (game_mode == SCREEN_SAVER) {
      scene->UpdateCamera();
    } else if (game_mode == PLAYING || game_mode == CREDITS) {
      //Collect keyboard input
      const float force_lr =
        (all_keys[sf::Keyboard::Left] || all_keys[sf::Keyboard::A] ? -1.0f : 0.0f) +
        (all_keys[sf::Keyboard::Right] || all_keys[sf::Keyboard::D] ? 1.0f : 0.0f);
      const float force_ud =
        (all_keys[sf::Keyboard::Down] || all_keys[sf::Keyboard::S] ? -1.0f : 0.0f) +
        (all_keys[sf::Keyboard::Up] || all_keys[sf::Keyboard::W] ? 1.0f : 0.0f);

      //Collect mouse input
      const sf::Vector2i mouse_delta = mouse_pos - *screen_center;
      sf::Mouse::setPosition(*screen_center, *window);
      float ms = mouse_sensitivity;
      if (mouse_setting == 1) {
        ms *= 0.5f;
      } else if (mouse_setting == 2) {
        ms *= 0.25f;
      }
      const float cam_lr = float(-mouse_delta.x) * ms;
      const float cam_ud = float(-mouse_delta.y) * ms;
      const float cam_z = mouse_wheel * wheel_sensitivity;

      //Apply forces to marble and camera
      scene->UpdateMarble(force_lr, force_ud);
      scene->UpdateCamera(cam_lr, cam_ud, cam_z);
    } else if (game_mode == PAUSED) {
      overlays->UpdatePaused((float)mouse_pos.x, (float)mouse_pos.y);
    }

    bool skip_frame = false;
    if (lag_ms >= 16) {
      //If there is too much lag, just do another frame of physics and skip the draw
      lag_ms -= 16;
      skip_frame = true;
    } else {
      //Update the shader values
      scene->Write(shader);

      //Setup full-screen shader
      sf::RenderStates states = sf::RenderStates::Default;
      states.shader = &shader;
      sf::RectangleShape rect;
      rect.setSize(*window_res);
      rect.setPosition(0, 0);

      //Draw the fractal
      if (fullscreen) {
        //Draw to the render texture
        renderTexture.draw(rect, states);
        renderTexture.display();

        //Draw render texture to main window
        sf::Sprite sprite(renderTexture.getTexture());
        sprite.setScale(float(screen_size.width) / float(resolution->width),
                        float(screen_size.height) / float(resolution->height));
        window->draw(sprite);
      } else {
        //Draw directly to the main window
        window->draw(rect, states);
      }
    }

    //Draw text overlays to the window
    if (game_mode == MAIN_MENU) {
      overlays->DrawMenu(*window);
    } else if (game_mode == CONTROLS) {
      overlays->DrawControls(*window);
    } else if (game_mode == LEVELS) {
      overlays->DrawLevels(*window);
    } else if (game_mode == PLAYING) {
      if (scene->GetMode() == Camera::ORBIT && scene->GetMarble().GetPosition().x() < 998.0f) {
        overlays->DrawLevelDesc(*window, scene->GetLevel());
      } else if (scene->GetMode() == Camera::MARBLE) {
        overlays->DrawArrow(*window, scene->GetGoalDirection());
      }
      overlays->DrawTimer(*window, scene->GetCountdownTime(), scene->IsHighScore());
    } else if (game_mode == PAUSED) {
      overlays->DrawPaused(*window);
    } else if (game_mode == CREDITS) {
      overlays->DrawCredits(*window);
    }
    overlays->DrawFPS(*window, int(smooth_fps + 0.5f));

    if (!skip_frame) {
      //Finally display to the screen
      window->display();

      //If V-Sync is running higher than desired fps, slow down!
      const float s = clock.restart().asSeconds();
      if (s > 0.0f) {
        smooth_fps = smooth_fps*0.9f + std::min(1.0f / s, 60.0f)*0.1f;
      }
      const int time_diff_ms = int(16.66667f - s*1000.0f);
      if (time_diff_ms > 0) {
        sf::sleep(sf::milliseconds(time_diff_ms));
        lag_ms = std::max(lag_ms - time_diff_ms, 0);
      } else if (time_diff_ms < 0) {
        lag_ms += std::max(-time_diff_ms, 0);
      }
    }
  }

  //Stop all music
  menu_music.stop();
  level1_music.stop();
  level2_music.stop();
  credits_music.stop();
  high_scores.Save(save_file);
}


float Game::GetVol() {
  if (!music_on) {
    return 0.0f;
  } else if (game_mode == PAUSED) {
    return music_vol / 4;
  } else {
    return music_vol;
  }
}



void Game::LockMouse(sf::RenderWindow& window) {
  window.setMouseCursorVisible(false);
  const sf::Vector2u size = window.getSize();
  mouse_pos = sf::Vector2i(size.x / 2, size.y / 2);
  sf::Mouse::setPosition(mouse_pos);
}
void Game::UnlockMouse(sf::RenderWindow& window) {
  window.setMouseCursorVisible(true);
}
int Game::dirExists(const char *path) {
  struct stat info;
  if (stat(path, &info) != 0) {
    return 0;
  } else if (info.st_mode & S_IFDIR) {
    return 1;
  }
  return 0;
}
