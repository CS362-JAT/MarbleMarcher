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
#pragma once
#include "Level.h"
#include "Marble.h"
#include "Camera.h"
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <Eigen/Dense>

class Scene {
public:
  
  Scene(sf::Music* m1, sf::Music* m2);

  void LoadLevel(int level);
  void SetMarble(float x, float y, float z, float r);
  void SetFlagPosition(float x, float y, float z);
  void SetMode(CamMode mode);
  void SetExposure(float e) { exposure = e; }
  void SetTimer(int t) { timer = t; }
  void SetSinglePlay(bool b) { play_single = b; }
  void SetLevel(int level) { cur_level = level; }

  const Marble GetMarble() const { return marble; }
  Eigen::Vector3f GetFlagPosition() const { return flag_pos; }
  float GetCamLook() const { return camera.GetLookXSmooth(); }
  float GetCamLookX() const { return camera.GetLookX(); }
  CamMode GetMode() const { return camera.GetMode(); }
  int GetTimer() const { return timer; }
  int GetLevel() const { return cur_level; }
  int GetCountdownTime() const;
  sf::Vector3f GetGoalDirection() const;
  float GetExposure() { return exposure; }
  bool IsSinglePlay() const { return play_single; }
  bool IsHighScore() const;
  int GetCurLevel() const { return cur_level; }
  Camera GetCamera() const { return camera; }

  sf::Music& GetCurMusic() const;
  void StopAllMusic();

  void StartNewGame();
  void StartNextLevel();
  void StartSingle(int level);
  void ResetLevel();

  void UpdateMarble(float dx=0.0f, float dy=0.0f);
  void UpdateCamera(float dx=0.0f, float dy=0.0f, float dz=0.0f);

  void SnapCamera();
  void HideObjects();

  void Write(sf::Shader& shader) const;

  float DE(const Eigen::Vector3f& pt) const;
  Eigen::Vector3f NP(const Eigen::Vector3f& pt) const;
  bool MarbleCollision(float& delta_v);

  void CheckIfMarbleHasHitFlag();
  void UpdateAnimatedFractals();
  void AddForceFromKeyboard(bool onGround, float dx, float dy);
  void PlayBounceSound(float max_delta_v);
  void ApplyGravityAndCollision(bool &onGround, float &max_delta_v);
  void NormalizeForce(float &dx, float &dy);
  void UpdateDemoFractal();

protected:
  void UpdateIntro(bool ssaver);
  void UpdateOrbit();
  void UpdateDeOrbit();
  void UpdateNormal(float dx, float dy, float dz);
  void UpdateGoal();
  void MakeCameraRotation();

private:
  int             cur_level;
  bool            intro_needs_snap;
  bool            play_single;

  Camera          camera;

  Marble          marble;

  Eigen::Vector3f flag_pos;

  FractalParams   frac_params;
  FractalParams   frac_params_smooth;

  int             timer;
  int             final_time;
  float           exposure;

  sf::Sound sound_goal;
  sf::SoundBuffer buff_goal;
  sf::Sound sound_bounce1;
  sf::SoundBuffer buff_bounce1;
  sf::Sound sound_bounce2;
  sf::SoundBuffer buff_bounce2;
  sf::Sound sound_bounce3;
  sf::SoundBuffer buff_bounce3;
  sf::Sound sound_shatter;
  sf::SoundBuffer buff_shatter;

  sf::Music* music_1;
  sf::Music* music_2;
};
