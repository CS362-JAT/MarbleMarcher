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
#include "Scene.h"
#include "Scores.h"
#include "Res.h"
#include "Marble.h"
#include <iostream>

static const float pi = 3.14159265359f;
static const float ground_force = 0.008f;
static const float air_force = 0.004f;
static const float ground_friction = 0.99f;
static const float air_friction = 0.995f;
static const float orbit_speed = 0.005f;
static const int max_marches = 10;
static const int num_phys_steps = 6;
static const float marble_bounce = 1.2f; //Range 1.0 to 2.0
static const float orbit_smooth = 0.995f;
static const float zoom_smooth = 0.85f;
static const float look_smooth = 0.75f;
static const int frame_transition = 400;
static const int frame_orbit = 600;
static const int frame_deorbit = 800;
static const int frame_countdown = frame_deorbit + 3*60;
static const float default_zoom = 15.0f;
static const int fractal_iters = 16;
static const float gravity = 0.005f;
static const float ground_ratio = 1.15f;
static const int mus_switch_lev = 9;

static void ModPi(float& a, float b) {
  if (a - b > pi) {
    a -= 2 * pi;
  } else if (a - b < -pi) {
    a += 2 * pi;
  }
}

Scene::Scene(sf::Music* m1, sf::Music* m2) :
  intro_needs_snap(true),
  play_single(false),
  exposure(1.0f),
  camera(Camera()),
  marble(Marble()),
  flag_pos(0.0f, 0.0f, 0.0f),
  timer(0),
  final_time(0),
  music_1(m1),
  music_2(m2),
  cur_level(0) {
  camera.SetDistance(default_zoom);
  frac_params.setOnes();
  frac_params_smooth.setOnes();
  SnapCamera();
  buff_goal.loadFromFile(goal_wav);
  sound_goal.setBuffer(buff_goal);
  buff_bounce1.loadFromFile(bounce1_wav);
  sound_bounce1.setBuffer(buff_bounce1);
  buff_bounce2.loadFromFile(bounce2_wav);
  sound_bounce2.setBuffer(buff_bounce2);
  buff_bounce3.loadFromFile(bounce3_wav);
  sound_bounce3.setBuffer(buff_bounce3);
  buff_shatter.loadFromFile(shatter_wav);
  sound_shatter.setBuffer(buff_shatter);
}

void Scene::LoadLevel(int level) {
  cur_level = level;
  marble.SetPosition(all_levels[level].start_pos);
  marble.SetRadius(all_levels[level].marble_rad);
  flag_pos = all_levels[level].end_pos;
  camera.SetLookX(all_levels[level].start_look_x);
}

void Scene::SetMarble(float x, float y, float z, float r) {
  marble.SetRadius(r);
  marble.SetPosition(Eigen::Vector3f(x, y, z));
  marble.SetVelocity(marble.GetVelocity().setZero());
}

void Scene::SetFlagPosition(float x, float y, float z) {
  flag_pos = Eigen::Vector3f(x, y, z);
}

void Scene::SetMode(CamMode mode) {
  //Don't reset the timer if transitioning to screen saver
  if ((camera.GetMode() == INTRO && mode == SCREEN_SAVER) ||
      (camera.GetMode() == SCREEN_SAVER && mode == INTRO)) {
  } else {
    timer = 0;
    intro_needs_snap = true;
  }
  camera.SetMode(mode);
}

int Scene::GetCountdownTime() const {
  if (camera.GetMode() == DEORBIT && timer >= frame_deorbit) {
    return timer - frame_deorbit;
  } else if (camera.GetMode() == MARBLE) {
    return timer + 3*60;
  } else if (camera.GetMode() == GOAL) {
    return final_time + 3*60;
  } else {
    return -1;
  }
}

sf::Vector3f Scene::GetGoalDirection() const {
  Eigen::Vector3f goal_delta = (marble.GetMatrix().transpose()) * (flag_pos - marble.GetPosition());
  goal_delta.y() = 0.0f;
  const float goal_dir = std::atan2(-goal_delta.z(), goal_delta.x());
  const float a = camera.GetLookX() - goal_dir;
  const float b = std::abs(camera.GetLookY() * 2.0f / pi);
  const float d = goal_delta.norm() / marble.GetRadius();
  return sf::Vector3f(a, b, d);
}

sf::Music& Scene::GetCurMusic() const {
  return *(cur_level < mus_switch_lev ? music_1 : music_2);
}

void Scene::StopAllMusic() {
  music_1->stop();
  music_2->stop();
}

bool Scene::IsHighScore() const {
  if (camera.GetMode() != GOAL) {
    return false;
  } else {
    return final_time == high_scores.Get(cur_level);
  }
}

void Scene::StartNewGame() {
  play_single = false;
  cur_level = high_scores.GetStartLevel();
  HideObjects();
  SetMode(ORBIT);
}

void Scene::StartNextLevel() {
  if (play_single) {
    camera.SetMode(MARBLE);
    ResetLevel();
  } else if (cur_level + 1 >= num_levels) {
    camera.SetMode(FINAL);
  } else {
    cur_level += 1;
    HideObjects();
    SetMode(ORBIT);
    if (cur_level == mus_switch_lev) {
      music_1->stop();
      music_2->play();
    }
  }
}

void Scene::StartSingle(int level) {
  play_single = true;
  cur_level = level;
  HideObjects();
  SetMode(ORBIT);
}

void Scene::ResetLevel() {
  if (camera.GetMode() == MARBLE || play_single) {
    SetMode(DEORBIT);
    timer = frame_deorbit;
    frac_params = all_levels[cur_level].params;
    frac_params_smooth = frac_params;
    marble.SetPosition(all_levels[cur_level].start_pos);
    marble.SetVelocity(marble.GetVelocity().setZero());
    marble.SetRadius(all_levels[cur_level].marble_rad);
    marble.SetMatrix(marble.GetMatrix().setIdentity());
    flag_pos = all_levels[cur_level].end_pos;
    camera.SetLookX(all_levels[cur_level].start_look_x);
    camera.SetLookXSmooth(camera.GetLookX());
    camera.SetPosition(camera.GetPositionSmooth());
    camera.SetDistance(default_zoom);
    camera.SetDistanceSmooth(camera.GetDistance());
    camera.SetLookY(-0.3f);
    camera.SetLookYSmooth(camera.GetLookY());
  }
}

void Scene::UpdateCamera(float dx, float dy, float dz) {
  //Camera update depends on current mode
  if (camera.GetMode() == INTRO) {
    UpdateIntro(false);
  } else if (camera.GetMode() == SCREEN_SAVER) {
    UpdateIntro(true);
  } else if (camera.GetMode() == ORBIT) {
    UpdateOrbit();
  } else if (camera.GetMode() == DEORBIT) {
    UpdateDeOrbit();
  } else if (camera.GetMode() == MARBLE) {
    UpdateNormal(dx, dy, dz);
  } else if (camera.GetMode() == GOAL || camera.GetMode() == FINAL) {
    UpdateGoal();
  }
}

void Scene::UpdateMarble(float dx, float dy) {
  //Ignore other modes
  if (camera.GetMode() != MARBLE) {
    return;
  }

  //Normalize force if too big
  NormalizeForce(dx, dy);

  bool onGround = false;
  float max_delta_v = 0.0f;
  ApplyGravityAndCollision(onGround, max_delta_v);
  PlayBounceSound(max_delta_v);
  AddForceFromKeyboard(onGround, dx, dy);

  //Apply friction
  marble.SetVelocity( marble.GetVelocity() * (onGround ? ground_friction : air_friction) );

  UpdateAnimatedFractals();
  CheckIfMarbleHasHitFlag();

  //Check if marble passed the death barrier
  if (marble.GetPosition().y() < all_levels[cur_level].kill_y) {
    ResetLevel();
  }
}

void Scene::UpdateIntro(bool ssaver) {
  //Update the timer
  const float t = -2.0f + timer * 0.002f;
  timer += 1;

  //Get rotational parameters
  const float dist = (ssaver ? 10.0f : 8.0f);
  const Eigen::Vector3f orbit_pt(0.0f, 3.0f, 0.0f);
  const Eigen::Vector3f perp_vec(std::sin(t), 0.0f, std::cos(t));
  camera.SetPosition(orbit_pt + perp_vec * dist);
  camera.SetPositionSmooth(camera.GetPositionSmooth()*0.9f + camera.GetPosition()*0.1f);

  //Solve for the look direction
  camera.SetLookX(std::atan2(perp_vec.x(), perp_vec.z()));
  if (!ssaver) { camera.SetLookX(camera.GetLookX() + 0.5f); }
  float cam_look_x_smooth = camera.GetLookXSmooth();
  ModPi(cam_look_x_smooth, camera.GetLookX());
  camera.SetLookXSmooth(cam_look_x_smooth*0.9f + camera.GetLookX()*0.1f);

  //Update look y
  camera.SetLookY(ssaver ? -0.25f : -0.41f);
  camera.SetLookYSmooth(camera.GetLookYSmooth()*0.9f + camera.GetLookY()*0.1f);

  //Update the camera matrix
  marble.SetMatrix(marble.GetMatrix().setIdentity());
  MakeCameraRotation();
  Eigen::Matrix4f cam_mat = camera.GetMatrix();
  cam_mat.block<3, 1>(0, 3) = camera.GetPositionSmooth();
  camera.SetMatrix(cam_mat);

  UpdateDemoFractal();

  //Make sure marble and flag are hidden
  HideObjects();

  if (intro_needs_snap) {
    SnapCamera();
    intro_needs_snap = false;
  }
}

void Scene::UpdateOrbit() {
  //Update the timer
  const float t = timer * orbit_speed;
  float a = std::min(float(timer) / float(frame_transition), 1.0f);
  a *= a/(2*a*(a - 1) + 1);
  timer += 1;

  //Get marble location and rotational parameters
  const float orbit_dist = all_levels[cur_level].orbit_dist;
  const Eigen::Vector3f orbit_pt(0.0f, orbit_dist, 0.0f);
  const Eigen::Vector3f perp_vec(std::sin(t), 0.0f, std::cos(t));
  camera.SetPosition(orbit_pt + perp_vec * (orbit_dist * 2.5f));
  camera.SetPositionSmooth(camera.GetPositionSmooth()*orbit_smooth + camera.GetPosition()*(1 - orbit_smooth));
  
  //Solve for the look direction
  camera.SetLookX(std::atan2(camera.GetPositionSmooth().x(), camera.GetPositionSmooth().z()));
  float aa = camera.GetLookXSmooth();
  ModPi(aa, camera.GetLookX());
  camera.SetLookXSmooth(aa);
  camera.SetLookXSmooth(camera.GetLookXSmooth()*(1 - a) + camera.GetLookX()*a);
  
  //Update look smoothing
  camera.SetLookY(-0.3f);
  camera.SetLookYSmooth(camera.GetLookYSmooth()*orbit_smooth + camera.GetLookY()*(1 - orbit_smooth));
  
  //Update the camera matrix
  marble.SetMatrix(marble.GetMatrix().setIdentity());
  MakeCameraRotation();
  Eigen::Matrix4f cam_mat = camera.GetMatrix();
  cam_mat.block<3, 1>(0, 3) = camera.GetPositionSmooth();
  camera.SetMatrix(cam_mat);

  //Update fractal parameters
  ModPi(frac_params[1], all_levels[cur_level].params[1]);
  ModPi(frac_params[2], all_levels[cur_level].params[2]);
  frac_params_smooth = frac_params * (1.0f - a) + all_levels[cur_level].params * a;

  //When done transitioning display the marble and flag
  if (timer >= frame_transition) {
    marble.SetPosition(all_levels[cur_level].start_pos);
    marble.SetRadius(all_levels[cur_level].marble_rad);
    flag_pos = all_levels[cur_level].end_pos;
  }

  //When done transitioning, setup level
  if (timer >= frame_orbit) {
    frac_params = all_levels[cur_level].params;
    camera.SetLookX(camera.GetLookXSmooth());
    camera.SetPosition(camera.GetPositionSmooth());
    camera.SetDistance(default_zoom);
    camera.SetDistanceSmooth(camera.GetDistance());
    camera.SetMode(DEORBIT);
  }
}

void Scene::UpdateDeOrbit() {
  //Update the timer
  const float t = timer * orbit_speed;
  float b = std::min(float(std::max(timer - frame_orbit, 0)) / float(frame_deorbit - frame_orbit), 1.0f);
  b *= b/(2*b*(b - 1) + 1);
  timer += 1;

  //Get marble location and rotational parameters
  const float orbit_dist = all_levels[cur_level].orbit_dist;
  const Eigen::Vector3f orbit_pt(0.0f, orbit_dist, 0.0f);
  const Eigen::Vector3f perp_vec(std::sin(t), 0.0f, std::cos(t));
  const Eigen::Vector3f orbit_cam_pos = orbit_pt + perp_vec * (orbit_dist * 2.5f);
  camera.SetPosition(camera.GetPosition()*orbit_smooth + orbit_cam_pos*(1 - orbit_smooth));

  //Solve for the look direction
  const float start_look_x = all_levels[cur_level].start_look_x;
  camera.SetLookX(std::atan2(camera.GetPosition().x(), camera.GetPosition().z()));
  float a = camera.GetLookX();
  ModPi(a, start_look_x);
  camera.SetLookX(a);

  //Solve for the look direction
  camera.SetLookXSmooth(camera.GetLookX()*(1 - b) + start_look_x*b);

  //Update look smoothing
  camera.SetLookY(-0.3f);
  camera.SetLookYSmooth(camera.GetLookYSmooth()*orbit_smooth + camera.GetLookY()*(1 - orbit_smooth));

  //Update the camera rotation matrix
  MakeCameraRotation();

  //Update the camera position
  Eigen::Vector3f marble_cam_pos = marble.GetPosition() + camera.GetMatrix().block<3, 3>(0, 0) * Eigen::Vector3f(0.0f, 0.0f, marble.GetRadius() * camera.GetDistanceSmooth());
  marble_cam_pos += Eigen::Vector3f(0.0f, marble.GetRadius() * camera.GetDistanceSmooth() * 0.1f, 0.0f);
  camera.SetPositionSmooth(camera.GetPosition()*(1 - b) + marble_cam_pos*b);
  Eigen::Matrix4f cam_mat = camera.GetMatrix();
  cam_mat.block<3, 1>(0, 3) = camera.GetPositionSmooth();
  camera.SetMatrix(cam_mat);

  //When done deorbiting, transition to play
  if (timer > frame_countdown) {
    camera.SetMode(MARBLE);
    camera.SetLookX(camera.GetLookXSmooth());
    camera.SetLookY(camera.GetLookYSmooth());
    camera.SetPosition(camera.GetPositionSmooth());
    timer = 0;
  }
}

void Scene::UpdateNormal(float dx, float dy, float dz) {
  //Update camera zoom
  camera.SetDistance(camera.GetDistance() * std::pow(2.0f, -dz));
  camera.SetDistance(std::min(std::max(camera.GetDistance(), 5.0f), 30.0f));
  camera.SetDistanceSmooth(camera.GetDistanceSmooth()*zoom_smooth + camera.GetDistance()*(1 - zoom_smooth));

  //Update look direction
  camera.SetLookX(camera.GetLookX() + dx);
  camera.SetLookY(camera.GetLookY() + dy);
  camera.SetLookY(std::min(std::max(camera.GetLookY(), -pi/2), pi/2));
  while (camera.GetLookX() > pi) { camera.SetLookX(camera.GetLookX() - 2*pi); }
  while (camera.GetLookX() < -pi) { camera.SetLookX(camera.GetLookX() + 2*pi); }

  //Update look smoothing
  float cam_look_x_smooth = camera.GetLookXSmooth();
  ModPi(cam_look_x_smooth, camera.GetLookX());
  cam_look_x_smooth = cam_look_x_smooth*look_smooth + camera.GetLookX()*(1 - look_smooth);
  camera.SetLookXSmooth(cam_look_x_smooth);
  camera.SetLookYSmooth(camera.GetLookYSmooth()*look_smooth + camera.GetLookY()*(1 - look_smooth));

  //Setup rotation matrix for planets
  if (all_levels[cur_level].planet) {
    Eigen::Matrix3f matrix = marble.GetMatrix();
    matrix.col(1) = marble.GetPosition().normalized();
    matrix.col(2) = -marble.GetMatrix().col(1).cross(marble.GetMatrix().col(0)).normalized();
    matrix.col(0) = -marble.GetMatrix().col(2).cross(marble.GetMatrix().col(1)).normalized();
    marble.SetMatrix(matrix);
  } else {
    marble.SetMatrix(marble.GetMatrix().setIdentity());
  }

  //Update the camera matrix
  MakeCameraRotation();
  camera.SetPosition(marble.GetPosition() + camera.GetMatrix().block<3, 3>(0, 0) * Eigen::Vector3f(0.0f, 0.0f, marble.GetRadius() * camera.GetDistanceSmooth()));
  camera.SetPosition(camera.GetPosition() + (marble.GetMatrix().col(1) * (marble.GetRadius() * camera.GetDistanceSmooth() * 0.1f)));
  camera.SetPositionSmooth(camera.GetPosition());
  Eigen::Matrix4f cam_mat = camera.GetMatrix();
  cam_mat.block<3, 1>(0, 3) = camera.GetPositionSmooth();
  camera.SetMatrix(cam_mat);

  //Update timer
  timer += 1;
}

void Scene::UpdateGoal() {
  //Update the timer
  const float t = timer * 0.01f;
  float a = std::min(t / 75.0f, 1.0f);
  timer += 1;

  //Get marble location and rotational parameters
  const float flag_dist = marble.GetRadius() * 6.5f;
  const Eigen::Vector3f orbit_pt = flag_pos + marble.GetMatrix() * Eigen::Vector3f(0.0f, flag_dist, 0.0f);
  const Eigen::Vector3f perp_vec = Eigen::Vector3f(std::sin(t), 0.0f, std::cos(t));
  camera.SetPosition(orbit_pt + marble.GetMatrix() * perp_vec * (flag_dist * 3.5f));
  camera.SetPositionSmooth(camera.GetPositionSmooth()*(1 - a) + camera.GetPosition()*a);

  //Solve for the look direction
  camera.SetLookX(std::atan2(perp_vec.x(), perp_vec.z()));
  float cam_look_x_smooth = camera.GetLookXSmooth();
  ModPi(cam_look_x_smooth, camera.GetLookX());
  cam_look_x_smooth = cam_look_x_smooth*(1 - a) + camera.GetLookX()*a;
  camera.SetLookXSmooth(cam_look_x_smooth);

  //Update look smoothing
  camera.SetLookY(-0.25f);
  camera.SetLookYSmooth(camera.GetLookYSmooth()*0.99f + camera.GetLookY()*(1 - 0.99f));

  //Update the camera matrix
  MakeCameraRotation();
  Eigen::Matrix4f cam_mat = camera.GetMatrix();
  cam_mat.block<3, 1>(0, 3) = camera.GetPositionSmooth();
  camera.SetMatrix(cam_mat);

  //Animate marble
  marble.SetVelocity(marble.GetVelocity() + ((orbit_pt - marble.GetPosition()) * 0.005f) );
  marble.SetPosition(marble.GetPosition() + marble.GetVelocity());
  if (marble.GetVelocity().norm() > marble.GetRadius()*0.02f) {
    marble.SetVelocity(marble.GetVelocity() * 0.95f);
  }

  if (timer > 300 && camera.GetMode() != FINAL) {
    StartNextLevel();
  }
}

void Scene::MakeCameraRotation() {
  camera.SetMatrix(camera.GetMatrix().setIdentity());
  const Eigen::AngleAxisf aa_x_smooth(camera.GetLookXSmooth(), Eigen::Vector3f::UnitY());
  const Eigen::AngleAxisf aa_y_smooth(camera.GetLookYSmooth(), Eigen::Vector3f::UnitX());
  Eigen::Matrix4f cam_mat = camera.GetMatrix();
  cam_mat.block<3, 3>(0, 0) = (marble.GetMatrix()) * (aa_x_smooth * aa_y_smooth).toRotationMatrix();
  camera.SetMatrix(cam_mat);
}

void Scene::SnapCamera() {
  camera.SetLookXSmooth(camera.GetLookX());
  camera.SetLookYSmooth(camera.GetLookY());
  camera.SetDistanceSmooth(camera.GetDistance());
  camera.SetPositionSmooth(camera.GetPosition());
}

void Scene::HideObjects() {
  marble.SetPosition(Eigen::Vector3f(999.0f, 999.0f, 999.0f));
  flag_pos = Eigen::Vector3f(999.0f, 999.0f, 999.0f);
  marble.SetVelocity(marble.GetVelocity().setZero());
}

void Scene::Write(sf::Shader& shader) const {
  shader.setUniform("iMat", sf::Glsl::Mat4(camera.GetMatrix().data()));

  shader.setUniform("iMarblePos", sf::Glsl::Vec3(marble.GetPosition().x(), marble.GetPosition().y(), marble.GetPosition().z()));
  shader.setUniform("iMarbleRad", marble.GetRadius());

  shader.setUniform("iFlagScale", all_levels[cur_level].planet ? -marble.GetRadius() : marble.GetRadius());
  shader.setUniform("iFlagPos", sf::Glsl::Vec3(flag_pos.x(), flag_pos.y(), flag_pos.z()));

  shader.setUniform("iFracScale", frac_params_smooth[0]);
  shader.setUniform("iFracAng1", frac_params_smooth[1]);
  shader.setUniform("iFracAng2", frac_params_smooth[2]);
  shader.setUniform("iFracShift", sf::Glsl::Vec3(frac_params_smooth[3], frac_params_smooth[4], frac_params_smooth[5]));
  shader.setUniform("iFracCol", sf::Glsl::Vec3(frac_params_smooth[6], frac_params_smooth[7], frac_params_smooth[8]));

  shader.setUniform("iExposure", exposure);
}

//Hard-coded to match the fractal
float Scene::DE(const Eigen::Vector3f& pt) const {
  //Easier to work with names
  const float frac_scale = frac_params_smooth[0];
  const float frac_angle1 = frac_params_smooth[1];
  const float frac_angle2 = frac_params_smooth[2];
  const Eigen::Vector3f frac_shift = frac_params_smooth.segment<3>(3);
  const Eigen::Vector3f frac_color = frac_params_smooth.segment<3>(6);

  Eigen::Vector4f p;
  p << pt, 1.0f;
  for (int i = 0; i < fractal_iters; ++i) {
    //absFold
    p.segment<3>(0) = p.segment<3>(0).cwiseAbs();
    //rotZ
    const float rotz_c = std::cos(frac_angle1);
    const float rotz_s = std::sin(frac_angle1);
    const float rotz_x = rotz_c*p.x() + rotz_s*p.y();
    const float rotz_y = rotz_c*p.y() - rotz_s*p.x();
    p.x() = rotz_x; p.y() = rotz_y;
    //mengerFold
    float a = std::min(p.x() - p.y(), 0.0f);
    p.x() -= a; p.y() += a;
    a = std::min(p.x() - p.z(), 0.0f);
    p.x() -= a; p.z() += a;
    a = std::min(p.y() - p.z(), 0.0f);
    p.y() -= a; p.z() += a;
    //rotX
    const float rotx_c = std::cos(frac_angle2);
    const float rotx_s = std::sin(frac_angle2);
    const float rotx_y = rotx_c*p.y() + rotx_s*p.z();
    const float rotx_z = rotx_c*p.z() - rotx_s*p.y();
    p.y() = rotx_y; p.z() = rotx_z;
    //scaleTrans
    p *= frac_scale;
    p.segment<3>(0) += frac_shift;
  }
  const Eigen::Vector3f a = p.segment<3>(0).cwiseAbs() - Eigen::Vector3f(6.0f, 6.0f, 6.0f);
  return (std::min(std::max(std::max(a.x(), a.y()), a.z()), 0.0f) + a.cwiseMax(0.0f).norm()) / p.w();
}

//Hard-coded to match the fractal
Eigen::Vector3f Scene::NP(const Eigen::Vector3f& pt) const {
  //Easier to work with names
  const float frac_scale = frac_params_smooth[0];
  const float frac_angle1 = frac_params_smooth[1];
  const float frac_angle2 = frac_params_smooth[2];
  const Eigen::Vector3f frac_shift = frac_params_smooth.segment<3>(3);
  const Eigen::Vector3f frac_color = frac_params_smooth.segment<3>(6);

  static std::vector<Eigen::Vector4f, Eigen::aligned_allocator<Eigen::Vector4f>> p_hist;
  p_hist.clear();
  Eigen::Vector4f p;
  p << pt, 1.0f;
  //Fold the point, keeping history
  for (int i = 0; i < fractal_iters; ++i) {
    //absFold
    p_hist.push_back(p);
    p.segment<3>(0) = p.segment<3>(0).cwiseAbs();
    //rotZ
    const float rotz_c = std::cos(frac_angle1);
    const float rotz_s = std::sin(frac_angle1);
    const float rotz_x = rotz_c*p.x() + rotz_s*p.y();
    const float rotz_y = rotz_c*p.y() - rotz_s*p.x();
    p.x() = rotz_x; p.y() = rotz_y;
    //mengerFold
    p_hist.push_back(p);
    float a = std::min(p.x() - p.y(), 0.0f);
    p.x() -= a; p.y() += a;
    a = std::min(p.x() - p.z(), 0.0f);
    p.x() -= a; p.z() += a;
    a = std::min(p.y() - p.z(), 0.0f);
    p.y() -= a; p.z() += a;
    //rotX
    const float rotx_c = std::cos(frac_angle2);
    const float rotx_s = std::sin(frac_angle2);
    const float rotx_y = rotx_c*p.y() + rotx_s*p.z();
    const float rotx_z = rotx_c*p.z() - rotx_s*p.y();
    p.y() = rotx_y; p.z() = rotx_z;
    //scaleTrans
    p *= frac_scale;
    p.segment<3>(0) += frac_shift;
  }
  //Get the nearest point
  Eigen::Vector3f n = p.segment<3>(0).cwiseMax(-6.0f).cwiseMin(6.0f);
  //Then unfold the nearest point (reverse order)
  for (int i = 0; i < fractal_iters; ++i) {
    //scaleTrans
    n.segment<3>(0) -= frac_shift;
    n /= frac_scale;
    //rotX
    const float rotx_c = std::cos(-frac_angle2);
    const float rotx_s = std::sin(-frac_angle2);
    const float rotx_y = rotx_c*n.y() + rotx_s*n.z();
    const float rotx_z = rotx_c*n.z() - rotx_s*n.y();
    n.y() = rotx_y; n.z() = rotx_z;
    //mengerUnfold
    p = p_hist.back(); p_hist.pop_back();
    const float mx = std::max(p[0], p[1]);
    if (std::min(p[0], p[1]) < std::min(mx, p[2])) {
      std::swap(n[1], n[2]);
    }
    if (mx < p[2]) {
      std::swap(n[0], n[2]);
    }
    if (p[0] < p[1]) {
      std::swap(n[0], n[1]);
    }
    //rotZ
    const float rotz_c = std::cos(-frac_angle1);
    const float rotz_s = std::sin(-frac_angle1);
    const float rotz_x = rotz_c*n.x() + rotz_s*n.y();
    const float rotz_y = rotz_c*n.y() - rotz_s*n.x();
    n.x() = rotz_x; n.y() = rotz_y;
    //absUnfold
    p = p_hist.back(); p_hist.pop_back();
    if (p[0] < 0.0f) {
      n[0] = -n[0];
    }
    if (p[1] < 0.0f) {
      n[1] = -n[1];
    }
    if (p[2] < 0.0f) {
      n[2] = -n[2];
    }
  }
  return n;
}

bool Scene::MarbleCollision(float& delta_v) {
  //Check if the distance estimate indicates a collision
  const float de = DE(marble.GetPosition());
  if (de >= marble.GetRadius()) {
    return de < marble.GetRadius() * ground_ratio;
  }
  
  //Check if the marble has been crushed by the fractal
  if (de < marble.GetRadius() * 0.001f) {
    sound_shatter.play();
    Eigen::Vector3f pos = marble.GetPosition();
    pos.y() = -9999.0f;
    marble.SetPosition(pos);
    return false;
  }

  //Find the nearest point and compute offset
  const Eigen::Vector3f np = NP(marble.GetPosition());
  const Eigen::Vector3f d = np - marble.GetPosition();
  const Eigen::Vector3f dn = d.normalized();

  //Apply the offset to the marble's position and velocity
  const float dv = marble.GetVelocity().dot(dn);
  delta_v = std::max(delta_v, dv);
  marble.SetPosition(marble.GetPosition() - (dn * marble.GetRadius() - d) );
  marble.SetVelocity(marble.GetVelocity() - (dn * (dv * marble_bounce)) );
  return true;
}

void Scene::CheckIfMarbleHasHitFlag()
{
	if (camera.GetMode() != GOAL) {
		const bool flag_y_match = all_levels[cur_level].planet ?
			marble.GetPosition().y() <= flag_pos.y() && marble.GetPosition().y() >= flag_pos.y() - 7 * marble.GetRadius() :
			marble.GetPosition().y() >= flag_pos.y() && marble.GetPosition().y() <= flag_pos.y() + 7 * marble.GetRadius();
		if (flag_y_match) {
			const float fx = marble.GetPosition().x() - flag_pos.x();
			const float fz = marble.GetPosition().z() - flag_pos.z();
			if (fx*fx + fz * fz < 6 * marble.GetRadius()*marble.GetRadius()) {
				final_time = timer;
				high_scores.Update(cur_level, final_time);
				SetMode(GOAL);
				sound_goal.play();
			}
		}
	}
}

void Scene::UpdateAnimatedFractals()
{
	frac_params[1] = all_levels[cur_level].params[1] + all_levels[cur_level].anim_1 * std::sin(timer * 0.015f);
	frac_params[2] = all_levels[cur_level].params[2] + all_levels[cur_level].anim_2 * std::sin(timer * 0.015f);
	frac_params[4] = all_levels[cur_level].params[4] + all_levels[cur_level].anim_3 * std::sin(timer * 0.015f);
	frac_params_smooth = frac_params;
}

void Scene::AddForceFromKeyboard(bool onGround, float dx, float dy)
{
	const float f = marble.GetRadius() * (onGround ? ground_force : air_force);
	const float cs = std::cos(camera.GetLookX());
	const float sn = std::sin(camera.GetLookX());
	Eigen::Vector3f v(dx*cs - dy * sn, 0.0f, -dy * cs - dx * sn);
	marble.SetVelocity(marble.GetVelocity() + ((marble.GetMatrix() * v) * f));
}

void Scene::PlayBounceSound(float max_delta_v)
{
	if (max_delta_v > 0.01f) {
		sound_bounce1.play();
	}
	else if (max_delta_v > 0.005f) {
		sound_bounce2.play();
	}
	else if (max_delta_v > 0.002f) {
		sound_bounce3.setVolume(100.0f * (max_delta_v / 0.005f));
		sound_bounce3.play();
	}
}

void Scene::ApplyGravityAndCollision(bool &onGround, float &max_delta_v)
{
	for (int i = 0; i < num_phys_steps; ++i) {
		const float force = marble.GetRadius() * gravity / num_phys_steps;
		if (all_levels[cur_level].planet) {
			marble.SetVelocity(marble.GetVelocity() - (marble.GetPosition().normalized() * force));
		}
		else {
			Eigen::Vector3f vel = marble.GetVelocity();
			vel.y() -= force;
			marble.SetVelocity(vel);
		}
		marble.SetPosition(marble.GetPosition() + (marble.GetVelocity() / num_phys_steps));
		onGround |= MarbleCollision(max_delta_v);
	}
}

void Scene::NormalizeForce(float &dx, float &dy)
{
	const float mag2 = dx * dx + dy * dy;
	if (mag2 > 1.0f) {
		const float mag = std::sqrt(mag2);
		dx /= mag;
		dy /= mag;
	}
}

void Scene::UpdateDemoFractal()
{
	frac_params[0] = 1.6f;
	frac_params[1] = 2.0f + 0.5f*std::cos(timer * 0.0021f);
	frac_params[2] = pi + 0.5f*std::cos(timer * 0.000287f);
	frac_params[3] = -4.0f + 0.5f*std::sin(timer * 0.00161f);
	frac_params[4] = -1.0f + 0.1f*std::sin(timer * 0.00123f);
	frac_params[5] = -1.0f + 0.1f*std::cos(timer * 0.00137f);
	frac_params[6] = -0.2f;
	frac_params[7] = -0.1f;
	frac_params[8] = -0.6f;
	frac_params_smooth = frac_params;
}