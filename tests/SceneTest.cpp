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
#include "Marble.h"
#include "Camera.h"

TEST(SceneAccessorsMutators, Level)
{
	sf::Music m1;
	sf::Music m2;
	m1.openFromFile(level1_ogg);
	m2.openFromFile(level2_ogg);
	Scene s(&m1, &m2);

	s.LoadLevel(4);

	EXPECT_EQ(4, s.GetLevel());
	EXPECT_EQ(all_levels[4].start_pos, s.GetMarble().GetPosition());
	EXPECT_EQ(all_levels[4].marble_rad, s.GetMarble().GetRadius());
	EXPECT_EQ(all_levels[4].end_pos, s.GetFlagPosition());
	EXPECT_EQ(all_levels[4].start_look_x, s.GetCamLookX());
}

TEST(SceneAccessorsMutators, Marble)
{
	sf::Music m1;
	sf::Music m2;
	m1.openFromFile(level1_ogg);
	m2.openFromFile(level2_ogg);
	Scene s(&m1, &m2);

	s.SetMarble(30.0f, 20.0f, 10.0f, 5.0f);

	EXPECT_EQ(Eigen::Vector3f(30.0f, 20.0f, 10.0f), s.GetMarble().GetPosition());
	EXPECT_EQ(5.0f, s.GetMarble().GetRadius());
	EXPECT_EQ(Eigen::Vector3f(0.0f, 0.0f, 0.0f), s.GetMarble().GetVelocity());
	EXPECT_EQ(Eigen::Matrix3f::Identity(), s.GetMarble().GetMatrix());
}

TEST(SceneAccessorsMutators, Mode)
{
	sf::Music m1;
	sf::Music m2;
	m1.openFromFile(level1_ogg);
	m2.openFromFile(level2_ogg);
	Scene s(&m1, &m2);

	s.SetMode(CamMode::ORBIT);

	EXPECT_EQ(CamMode::ORBIT, s.GetMode());
	EXPECT_EQ(0, s.GetTimer());
}

TEST(SceneAccessorsMutators, Flag) {
	sf::Music m1;
	sf::Music m2;
	m1.openFromFile(level1_ogg);
	m2.openFromFile(level2_ogg);
	Scene s(&m1, &m2);

	s.SetFlagPosition(50.0f, 25.0f, 10.0f);

	EXPECT_EQ(Eigen::Vector3f(50.0f, 25.0f, 10.0f), s.GetFlagPosition());
}

TEST(SceneAccessorsMutators, Exposure) {
	sf::Music m1;
	sf::Music m2;
	m1.openFromFile(level1_ogg);
	m2.openFromFile(level2_ogg);
	Scene s(&m1, &m2);

	s.SetExposure(5.0f);

	EXPECT_EQ(5.0f, s.GetExposure());
}

TEST(SceneAccessorsMutators, IsSinglePlay)
{
	sf::Music m1;
	sf::Music m2;
	m1.openFromFile(level1_ogg);
	m2.openFromFile(level2_ogg);
	Scene s(&m1, &m2);
	
	EXPECT_EQ(false, s.IsSinglePlay());
}

TEST(SceneFunctions, Constructor) 
{
	sf::Music m1;
	sf::Music m2;
	m1.openFromFile(level1_ogg);
	m2.openFromFile(level2_ogg);

	Scene s(&m1, &m2);

	EXPECT_EQ(false, s.IsSinglePlay());
	EXPECT_EQ(1.0f, s.GetExposure());
	EXPECT_EQ(CamMode::INTRO, s.GetMode());
	EXPECT_EQ(1.0f, s.GetMarble().GetRadius());
	EXPECT_EQ(Eigen::Vector3f(0.0f, 0.0f, 0.0f), s.GetMarble().GetPosition());
	EXPECT_EQ(Eigen::Vector3f(0.0f, 0.0f, 0.0f), s.GetMarble().GetVelocity());
	EXPECT_EQ(Eigen::Matrix3f::Identity(), s.GetMarble().GetMatrix());
	EXPECT_EQ(Eigen::Vector3f(0.0f, 0.0f, 0.0f), s.GetFlagPosition());
	EXPECT_EQ(0, s.GetLevel());
	EXPECT_EQ(-1, s.GetCountdownTime());
	EXPECT_EQ(false, s.IsHighScore());
	EXPECT_EQ(m1.getDuration(), s.GetCurMusic().getDuration());
}

TEST(SceneFunctions, StartNewGame)
{
	sf::Music m1;
	sf::Music m2;
	m1.openFromFile(level1_ogg);
	m2.openFromFile(level2_ogg);
	Scene s(&m1, &m2);

	s.StartNewGame();

	EXPECT_EQ(false, s.IsSinglePlay());
	EXPECT_EQ(CamMode::ORBIT, s.GetMode());
	EXPECT_EQ(Eigen::Vector3f(999.0f, 999.0f, 999.0f), s.GetMarble().GetPosition());
	EXPECT_EQ(Eigen::Vector3f(999.0f, 999.0f, 999.0f), s.GetFlagPosition());
	EXPECT_EQ(Eigen::Vector3f(0.0f, 0.0f, 0.0f), s.GetMarble().GetVelocity());
}

TEST(SceneFunctions, StartNextLevel)
{
	sf::Music m1;
	sf::Music m2;
	m1.openFromFile(level1_ogg);
	m2.openFromFile(level2_ogg);
	Scene s(&m1, &m2);
	Scene s2(&m1, &m2);
	Scene s3(&m1, &m2);
	Scene s4(&m1, &m2);

	s2.SetSinglePlay(true);
	s3.SetLevel(20);
	s4.SetLevel(8);

	s.StartNextLevel();
	s2.StartNextLevel();
	s3.StartNextLevel();
	s4.StartNextLevel();

	EXPECT_EQ(CamMode::ORBIT, s.GetMode());
	EXPECT_EQ(Eigen::Vector3f(999.0f, 999.0f, 999.0f), s.GetMarble().GetPosition());
	EXPECT_EQ(Eigen::Vector3f(999.0f, 999.0f, 999.0f), s.GetFlagPosition());
	EXPECT_EQ(Eigen::Vector3f(0.0f, 0.0f, 0.0f), s.GetMarble().GetVelocity());
	EXPECT_EQ(1, s.GetCurLevel());
	EXPECT_EQ(CamMode::DEORBIT, s2.GetMode());
	EXPECT_EQ(all_levels[s2.GetCurLevel()].start_pos, s2.GetMarble().GetPosition());
	EXPECT_EQ(CamMode::FINAL, s3.GetMode());
	EXPECT_EQ(sf::SoundSource::Status::Stopped, m1.getStatus());
	EXPECT_EQ(sf::SoundSource::Status::Playing, m2.getStatus());
}

TEST(SceneFunctions, HideObjects)
{
	sf::Music m1;
	sf::Music m2;
	m1.openFromFile(level1_ogg);
	m2.openFromFile(level2_ogg);
	Scene s(&m1, &m2);

	s.HideObjects();

	EXPECT_EQ(Eigen::Vector3f(999.0f, 999.0f, 999.0f), s.GetMarble().GetPosition());
	EXPECT_EQ(Eigen::Vector3f(999.0f, 999.0f, 999.0f), s.GetFlagPosition());
	EXPECT_EQ(Eigen::Vector3f(0.0f, 0.0f, 0.0f), s.GetMarble().GetVelocity());
}

TEST(SceneFunctions, StartSingle)
{
	sf::Music m1;
	sf::Music m2;
	m1.openFromFile(level1_ogg);
	m2.openFromFile(level2_ogg);
	Scene s(&m1, &m2);

	s.StartSingle(7);

	EXPECT_EQ(7, s.GetCurLevel());
	EXPECT_EQ(true, s.IsSinglePlay());
	EXPECT_EQ(CamMode::ORBIT, s.GetMode());
	EXPECT_EQ(Eigen::Vector3f(999.0f, 999.0f, 999.0f), s.GetMarble().GetPosition());
	EXPECT_EQ(Eigen::Vector3f(999.0f, 999.0f, 999.0f), s.GetFlagPosition());
	EXPECT_EQ(Eigen::Vector3f(0.0f, 0.0f, 0.0f), s.GetMarble().GetVelocity());
}

TEST(SceneFunctions, StopAllMusic)
{
	sf::Music m1;
	sf::Music m2;
	m1.openFromFile(level1_ogg);
	m2.openFromFile(level2_ogg);
	Scene s(&m1, &m2);
	m1.play();
	m2.play();

	s.StopAllMusic();

	EXPECT_EQ(sf::SoundSource::Status::Stopped, m1.getStatus());
	EXPECT_EQ(sf::SoundSource::Status::Stopped, m2.getStatus());
}

TEST(SceneFunctions, ResetLevel)
{
	sf::Music m1;
	sf::Music m2;
	m1.openFromFile(level1_ogg);
	m2.openFromFile(level2_ogg);
	Scene s(&m1, &m2);
	s.StartSingle(6);

	s.ResetLevel();

	EXPECT_EQ(CamMode::DEORBIT, s.GetMode());
	EXPECT_EQ(all_levels[s.GetCurLevel()].start_pos, s.GetMarble().GetPosition());
	EXPECT_EQ(all_levels[s.GetCurLevel()].marble_rad, s.GetMarble().GetRadius());
	EXPECT_EQ(all_levels[s.GetCurLevel()].end_pos, s.GetFlagPosition());
	EXPECT_EQ(all_levels[s.GetCurLevel()].start_look_x, s.GetCamLookX());
	EXPECT_EQ(Eigen::Vector3f(0.0f, 0.0f, 0.0f), s.GetMarble().GetVelocity());
	EXPECT_EQ(Eigen::Matrix3f::Identity(), s.GetMarble().GetMatrix());
}

TEST(SceneFunctions, GetCountDownTime) {
	sf::Music m1;
	sf::Music m2;
	m1.openFromFile(level1_ogg);
	m2.openFromFile(level2_ogg);
	Scene s1(&m1, &m2);
	Scene s2(&m1, &m2);
	Scene s3(&m1, &m2);
	Scene s4(&m1, &m2);

	s1.SetMode(CamMode::DEORBIT);
	s1.SetTimer(900);
	s2.SetMode(CamMode::MARBLE);
	s3.SetMode(CamMode::GOAL);
	s4.SetMode(CamMode::ORBIT);

	EXPECT_EQ(100, s1.GetCountdownTime());
	EXPECT_EQ(180, s2.GetCountdownTime());
	EXPECT_EQ(180, s3.GetCountdownTime());
	EXPECT_EQ(-1, s4.GetCountdownTime());
}

TEST(SceneFunctions, GetGoalDirection) {
	sf::Music m1;
	sf::Music m2;
	m1.openFromFile(level1_ogg);
	m2.openFromFile(level2_ogg);
	Scene s(&m1, &m2);

	s.SetMarble(0.0f, 0.0f, 0.0f, 1.0f);
	s.SetFlagPosition(1.0f, 1.0f, 1.0f);

	EXPECT_EQ(sf::Vector3f(0.0f - std::atan2(-1.0f, 1.0f), 0.0f, std::sqrt(2.0f)), s.GetGoalDirection());
}

TEST(SceneFunctions, UpdateCamera) {
	sf::Music m1;
	sf::Music m2;
	m1.openFromFile(level1_ogg);
	m2.openFromFile(level2_ogg);
	Scene s1(&m1, &m2);
	Scene s2(&m1, &m2);
	Scene s3(&m1, &m2);
	Scene s4(&m1, &m2);
	Scene s5(&m1, &m2);

	s1.SetMode(CamMode::INTRO);
	s2.SetMode(CamMode::ORBIT);
	s3.SetMode(CamMode::DEORBIT);
	s4.SetMode(CamMode::MARBLE);
	s5.SetMode(CamMode::GOAL);

	s1.UpdateCamera(1.0f, 1.0f, 1.0f);
	s2.UpdateCamera(1.0f, 1.0f, 1.0f);
	s3.UpdateCamera(1.0f, 1.0f, 1.0f);
	s4.UpdateCamera(1.0f, 1.0f, 1.0f);
	s5.UpdateCamera(1.0f, 1.0f, 1.0f);

	EXPECT_EQ(1, s1.GetTimer());
	EXPECT_EQ(Eigen::Vector3f(999.0f, 999.0f, 999.0f), s1.GetMarble().GetPosition());
	EXPECT_EQ(Eigen::Vector3f(999.0f, 999.0f, 999.0f), s1.GetFlagPosition());
	EXPECT_EQ(Eigen::Vector3f(0.0f, 0.0f, 0.0f), s1.GetMarble().GetVelocity());
	EXPECT_EQ( (Eigen::Vector3f(0.0f, 3.0f, 0.0f) + Eigen::Vector3f(std::sin(-2.0f), 0.0f, std::cos(-2.0f)) * 8.0f) , s1.GetCamera().GetPosition());

	EXPECT_EQ(1, s2.GetTimer());
	EXPECT_EQ( (Eigen::Vector3f(0.0f, all_levels[s2.GetCurLevel()].orbit_dist, 0.0f) 
				+ Eigen::Vector3f(std::sin(0.0f), 0.0f, std::cos(0.0f)) * (all_levels[s2.GetCurLevel()].orbit_dist * 2.5f) ) , s2.GetCamera().GetPosition());

	EXPECT_EQ(1, s3.GetTimer());
	EXPECT_EQ( (Eigen::Vector3f(0.0f, 0.0f, 0.0f)) * 0.995f + ((Eigen::Vector3f(0.0f, all_levels[s2.GetCurLevel()].orbit_dist, 0.0f)
		+ Eigen::Vector3f(std::sin(0.0f), 0.0f, std::cos(0.0f)) * (all_levels[s2.GetCurLevel()].orbit_dist * 2.5f))) * (1 - 0.995f), s3.GetCamera().GetPosition());

	EXPECT_EQ(1, s4.GetTimer());

	EXPECT_EQ(1, s5.GetTimer());
	EXPECT_EQ(-0.25f, s5.GetCamera().GetLookY());
}