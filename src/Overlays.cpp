/* This file is part of the Marble Marcher (https://github.com/HackerPoet/MarbleMarcher).
* Copyright(C) 2018 CodeParade
* 
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 2 of the License, or
* (at your option) any later version.
* 
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
* 
* You should have received a copy of the GNU General Public License
* along with this program.If not, see <http://www.gnu.org/licenses/>.
*/
#include "Overlays.h"
#include "Level.h"
#include "Res.h"
#include "Scores.h"

int mouse_setting = 0;
bool music_on = true;

Overlays::Overlays()
{

}

Overlays::Overlays(const sf::Font* _font, const sf::Font* _font_mono) :
  font(_font),
  font_mono(_font_mono),
  draw_scale(1.0f),
  top_level(true) {
  memset(all_hover, 0, sizeof(all_hover));
  buff_hover.loadFromFile(menu_hover_wav);
  sound_hover.setBuffer(buff_hover);
  buff_click.loadFromFile(menu_click_wav);
  sound_click.setBuffer(buff_click);
  buff_count.loadFromFile(count_down_wav);
  sound_count.setBuffer(buff_count);
  buff_go.loadFromFile(count_go_wav);
  sound_go.setBuffer(buff_go);
  arrow_tex.loadFromFile(arrow_png);
  arrow_tex.setSmooth(true);
  arrow_spr.setTexture(arrow_tex);
  arrow_spr.setOrigin(arrow_spr.getLocalBounds().width / 2, arrow_spr.getLocalBounds().height / 2);
}

Overlays::Texts Overlays::GetOption(Texts from, Texts to) {
  for (int i = from; i <= to; ++i) {
    if (all_hover[i]) {
      sound_click.play();
      return Texts(i);
    }
  }
  return Texts::TITLE;
}

void Overlays::UpdateMenu(float mouse_x, float mouse_y) {
  //Update text boxes
  struct TextCharacteristics textInfo;
  textInfo.str = "Marble\nMarcher";
  textInfo.x = 60;
  textInfo.y = 20;
  textInfo.size = 72;
  textInfo.mono = false;
  MakeText(textInfo, sf::Color::White, all_text[TITLE]);
  textInfo.str = "Play";
  textInfo.x = 80;
  textInfo.y = 230;
  textInfo.size = 60;
  MakeText(textInfo, sf::Color::White, all_text[PLAY]);
  textInfo.str = "Levels";
  textInfo.y = 300;
  MakeText(textInfo, sf::Color::White, all_text[LEVELS]);
  textInfo.str = "Controls";
  textInfo.y = 370;
  MakeText(textInfo, sf::Color::White, all_text[CONTROLS]);
  textInfo.str = "Screen Saver";
  textInfo.y = 440;
  MakeText(textInfo, sf::Color::White, all_text[SCREEN_SAVER]);
  textInfo.str = "Exit";
  textInfo.y = 510;
  MakeText(textInfo, sf::Color::White, all_text[EXIT]);
  textInfo.str = "\xA9""2019 CodeParade\nMusic by PettyTheft";
  textInfo.x = 16;
  textInfo.y = 652;
  textInfo.size = 32;
  textInfo.mono = true;
  MakeText(textInfo, sf::Color::White, all_text[CREDITS]);
  all_text[TITLE].setLineSpacing(0.76f);
  all_text[CREDITS].setLineSpacing(0.9f);

  //Check if mouse intersects anything
  UpdateHover(PLAY, EXIT, mouse_x, mouse_y);
}

void Overlays::UpdateControls(float mouse_x, float mouse_y) {
  //Update text boxes
  struct TextCharacteristics textInfo;
  textInfo.str = "Roll\nCamera\nZoom\nRestart\nPause";
  textInfo.x = 40;
  textInfo.y = 200;
  textInfo.size = 46;
  textInfo.mono = false;
  MakeText(textInfo, sf::Color::White, all_text[CONTROLS_L]);
  textInfo.str = "WASD or Arrows\nMouse\nScroll Wheel\nR or Right-Click\nEsc";
  textInfo.x = 280;
  MakeText(textInfo, sf::Color::White, all_text[CONTROLS_R]);
  textInfo.str = "Back";
  textInfo.x = 60;
  textInfo.y = 550;
  textInfo.size = 40;
  MakeText(textInfo, sf::Color::White, all_text[BACK]);

  //A little extra vertical spacing
  all_text[CONTROLS_L].setLineSpacing(1.1f);
  all_text[CONTROLS_R].setLineSpacing(1.1f);

  //Check if mouse intersects anything
  UpdateHover(BACK, BACK, mouse_x, mouse_y);
}

void Overlays::UpdateLevels(float mouse_x, float mouse_y) {
  //Update text boxes
  struct TextCharacteristics textInfo;
  textInfo.str = "Back";
  textInfo.x = 590;
  textInfo.y = 660;
  textInfo.size = 40;
  textInfo.mono = false;
  for (int i = 0; i < num_levels; ++i) {
    const float y = 80.0f + float(i/3) * 120.0f;
    const float x = 240.0f + float(i%3) * 400.0f;
    const char* txt = high_scores.HasUnlocked(i) ? all_levels[i].txt : "???";
	textInfo.str = txt;
	textInfo.x = x;
	textInfo.y = y;
	textInfo.size = 32;
    MakeText(textInfo, sf::Color::White, all_text[i + L0]);
    const sf::FloatRect text_bounds = all_text[i + L0].getLocalBounds();
    all_text[i + L0].setOrigin(text_bounds.width / 2, text_bounds.height / 2);
  }
  MakeText(textInfo, sf::Color::White, all_text[BACK2]);

  //Check if mouse intersects anything
  UpdateHover(L0, BACK2, mouse_x, mouse_y);
}

void Overlays::UpdatePaused(float mouse_x, float mouse_y) {
  //Update text boxes
  struct TextCharacteristics textInfo;
  textInfo.str = "Paused";
  textInfo.x = 540;
  textInfo.y = 288;
  textInfo.size = 54;
  textInfo.mono = false;
  MakeText(textInfo, sf::Color::White, all_text[PAUSED]);
  textInfo.str = "Continue";
  textInfo.x = 370;
  textInfo.y = 356;
  MakeText(textInfo, sf::Color::White, all_text[CONTINUE]);
  textInfo.str = "Restart";
  textInfo.x = 620;
  MakeText(textInfo, sf::Color::White, all_text[RESTART]);
  textInfo.str = "Quit";
  textInfo.x = 845;
  MakeText(textInfo, sf::Color::White, all_text[QUIT]);

  //Update music setting
  const char* music_txt = (music_on ? "Music:  On" : "Music:  Off");
  textInfo.str = music_txt;
  textInfo.x = 410;
  textInfo.y = 500;
  textInfo.size = 40;
  MakeText(textInfo, sf::Color::White, all_text[MUSIC]);

  //Update mouse sensitivity setting
  const char* mouse_txt = "Mouse Sensitivity:  High";
  if (mouse_setting == 1) {
    mouse_txt = "Mouse Sensitivity:  Medium";
  } else if (mouse_setting == 2) {
    mouse_txt = "Mouse Sensitivity:  Low";
  }
  textInfo.str = mouse_txt;
  textInfo.x = 410;
  textInfo.y = 550;
  MakeText(textInfo, sf::Color::White, all_text[MOUSE]);

  //Check if mouse intersects anything
  UpdateHover(CONTINUE, MOUSE, mouse_x, mouse_y);
}

void Overlays::DrawMenu(sf::RenderWindow& window) {
  for (int i = TITLE; i <= CREDITS; ++i) {
    window.draw(all_text[i]);
  }
}

void Overlays::DrawControls(sf::RenderWindow& window) {
  for (int i = CONTROLS_L; i <= BACK; ++i) {
    window.draw(all_text[i]);
  }
}

void Overlays::DrawTimer(sf::RenderWindow& window, int t, bool is_high_score) {
  sf::Text text;
  struct TextCharacteristics textInfo;
  textInfo.x = 640;
  textInfo.y = 50;
  textInfo.size = 140;
  textInfo.mono = false;
  if (t < 0) {
    return;
  } else if (t < 3*60) {
    //Create text for the number
    char txt[] = "0";
    txt[0] = '3' - (t / 60);
	textInfo.str = txt;
    MakeText(textInfo, sf::Color::White, text);

    //Play count sound if needed
    if (t % 60 == 0) {
      sound_count.play();
    }
  } else if (t < 4*60) {
    textInfo.str = "GO!";
    MakeText(textInfo, sf::Color::White, text);

    //Play go sound if needed
    if (t == 3*60) {
      sound_go.play();
    }
  } else {
    //Create timer text
    const int score = t - 3 * 60;
    const sf::Color col = (is_high_score ? sf::Color::Green : sf::Color::White);
    MakeTime(score, 530, 10, 60, col, text);
  }

  if (t < 4*60) {
    //Apply zoom animation
    const float fpart = float(t % 60) / 60.0f;
    const float zoom = 0.8f + fpart*0.2f;
    const sf::Uint8 alpha = sf::Uint8(255.0f*(1.0f - fpart*fpart));
    text.setScale(sf::Vector2f(zoom, zoom));
    text.setFillColor(sf::Color(255, 255, 255, alpha));
    text.setOutlineColor(sf::Color(0, 0, 0, alpha));

    //Center text
    const sf::FloatRect text_bounds = text.getLocalBounds();
    text.setOrigin(text_bounds.width / 2, text_bounds.height / 2);
  }

  //Draw the text
  window.draw(text);
}

void Overlays::DrawLevelDesc(sf::RenderWindow& window, int level) {
  sf::Text text;
  struct TextCharacteristics textInfo;
  textInfo.str = all_levels[level].txt;
  textInfo.x = 640;
  textInfo.y = 60;
  textInfo.size = 48;
  textInfo.mono = false;
  MakeText(textInfo, sf::Color::White, text);
  const sf::FloatRect text_bounds = text.getLocalBounds();
  text.setOrigin(text_bounds.width / 2, text_bounds.height / 2);
  window.draw(text);
}

void Overlays::DrawFPS(sf::RenderWindow& window, int fps) {
  sf::Text text;
  std::string fps_str = std::to_string(fps) + "fps";
  const sf::Color col = (fps < 50 ? sf::Color::Red : sf::Color::White);
  struct TextCharacteristics textInfo;
  textInfo.str = fps_str.c_str();
  textInfo.x = 1280;
  textInfo.y = 720;
  textInfo.size = 24;
  textInfo.mono = false;
  MakeText(textInfo, col, text);
  const sf::FloatRect text_bounds = text.getLocalBounds();
  text.setOrigin(text_bounds.width, text_bounds.height);
  window.draw(text);
}

void Overlays::DrawPaused(sf::RenderWindow& window) {
  for (int i = PAUSED; i <= MOUSE; ++i) {
    window.draw(all_text[i]);
  }
}

void Overlays::DrawArrow(sf::RenderWindow& window, const sf::Vector3f& v3) {
  const float x_scale = 250.0f * v3.y + 520.0f * (1.0f - v3.y);
  const float x = 640.0f + x_scale * std::cos(v3.x);
  const float y = 360.0f + 250.0f * std::sin(v3.x);
  float pi = 3.14159265359f;
  const sf::Uint8 alpha = sf::Uint8(102.0f * std::max(0.0f, std::min(1.0f, (v3.z - 5.0f) / 30.0f)));
  if (alpha > 0) {
    arrow_spr.setScale(draw_scale * 0.1f, draw_scale * 0.1f);
    arrow_spr.setRotation(90.0f + v3.x * 180.0f / pi);
    arrow_spr.setPosition(draw_scale * x, draw_scale * y);
    arrow_spr.setColor(sf::Color(255, 255, 255, alpha));
    window.draw(arrow_spr);
  }
}

void Overlays::DrawCredits(sf::RenderWindow& window) {
  const char* txt =
    "Congratulations, you beat all the levels!\n\n"
    "I hope it was as fun to play this demo as\n"
    "it was to make it. For more information about\n"
    "this game and other projects, check out my\n"
    "YouTube channel \"CodeParade\".\n\n"
    "Thanks for playing!";
  sf::Text text;
  struct TextCharacteristics textInfo;
  textInfo.str = txt;
  textInfo.x = 50;
  textInfo.y = 100;
  textInfo.size = 44;
  textInfo.mono = false;
  MakeText(textInfo, sf::Color::White, text);
  text.setLineSpacing(1.3f);
  window.draw(text);
}

void Overlays::DrawLevels(sf::RenderWindow& window) {
  //Draw the level names
  for (int i = L0; i <= BACK2; ++i) {
    window.draw(all_text[i]);
  }
  //Draw the times
  for (int i = 0; i < num_levels; ++i) {
    if (high_scores.HasCompleted(i)) {
      sf::Text text;
      const float y = 98.0f + float(i / 3) * 120.0f;
      const float x = 148.0f + float(i % 3) * 400.0f;
      MakeTime(high_scores.Get(i), x, y, 48, sf::Color(64, 255, 64), text);
      window.draw(text);
    }
  }
}

bool* Overlays::getAllHover()
{
	return all_hover;
}

void Overlays::MakeText(TextCharacteristics textInfo, sf::Color color, sf::Text& text) {
  text.setString(textInfo.str);
  text.setFont(textInfo.mono ? *font_mono : *font);
  text.setCharacterSize(int(textInfo.size * draw_scale));
  text.setLetterSpacing(0.8f);
  text.setPosition((textInfo.x - 2.0f) * draw_scale, (textInfo.y - 2.0f) * draw_scale);
  text.setFillColor(color);
  text.setOutlineThickness(3.0f * draw_scale);
  text.setOutlineColor(sf::Color::Black);
}

void Overlays::MakeTime(int t, float x, float y, float size, const sf::Color& color, sf::Text& text) {
  //Create timer text
  struct TextCharacteristics textInfo;
  textInfo.x = x;
  textInfo.y = y;
  textInfo.size = size;
  textInfo.mono = true;
  char txt[] = "00:00:00";
  const int t_all = std::min(t, 59 * (60 * 60 + 60 + 1));
  const int t_ms = t_all % 60;
  const int t_sec = (t_all / 60) % 60;
  const int t_min = t_all / (60 * 60);
  txt[0] = '0' + t_min / 10; txt[1] = '0' + t_min % 10;
  txt[3] = '0' + t_sec / 10; txt[4] = '0' + t_sec % 10;
  txt[6] = '0' + t_ms / 10;  txt[7] = '0' + t_ms % 10;
  textInfo.str = txt;
  MakeText(textInfo, color, text);
}

void Overlays::UpdateHover(Texts from, Texts to, float mouse_x, float mouse_y) {
  for (int i = from; i <= to; ++i) {
    const sf::FloatRect bounds = all_text[i].getGlobalBounds();
    if (bounds.contains(mouse_x, mouse_y)) {
      all_text[i].setFillColor(sf::Color(255, 64, 64));
      if (!all_hover[i]) {
        sound_hover.play();
        all_hover[i] = true;
      }
    } else {
      all_hover[i] = false;
    }
  }
}
