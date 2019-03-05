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

TEST(SceneAccessorsMutators, Level)
{
	sf::Music m1;
	sf::Music m2;
	m1.openFromFile(level1_ogg);
	m2.openFromFile(level2_ogg);
	Scene s(&m1, &m2);

	s.LoadLevel(4);

	EXPECT_EQ(4, s.GetLevel());
}

TEST(SceneAccessorsMutators, Marble)
{
	sf::Music m1;
	sf::Music m2;
	m1.openFromFile(level1_ogg);
	m2.openFromFile(level2_ogg);
	Scene s(&m1, &m2);

	s.SetMarble(30.0f, 20.0f, 10.0f, 5.0f);

	EXPECT_EQ(Eigen::Vector3f(30.0f, 20.0f, 10.0f), s.GetMarble());
}

TEST(SceneAccessorsMutators, Mode)
{
	sf::Music m1;
	sf::Music m2;
	m1.openFromFile(level1_ogg);
	m2.openFromFile(level2_ogg);
	Scene s(&m1, &m2);

	s.SetMode(Scene::CamMode::ORBIT);

	EXPECT_EQ(Scene::CamMode::ORBIT, s.GetMode());
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

	EXPECT_EQ(Eigen::Vector3f(0.0f, 0.0f, 0.0f), s.GetMarble());
	EXPECT_EQ(Scene::CamMode::INTRO, s.GetMode());
	EXPECT_EQ(0, s.GetLevel());
	EXPECT_EQ(-1, s.GetCountdownTime());
	EXPECT_EQ(false, s.IsSinglePlay());
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
	EXPECT_EQ(Scene::CamMode::ORBIT, s.GetMode());
	EXPECT_EQ(Eigen::Vector3f(999.0f, 999.0f, 999.0f), s.GetMarble());
}

TEST(SceneFunctions, StartNextLevel)
{
	sf::Music m1;
	sf::Music m2;
	m1.openFromFile(level1_ogg);
	m2.openFromFile(level2_ogg);
	Scene s(&m1, &m2);
	s.SetMarble(0.0f, 0.0f, 0.0f, 5.0f);

	s.StartNextLevel();

	EXPECT_EQ(Scene::CamMode::ORBIT, s.GetMode());
	EXPECT_EQ(Eigen::Vector3f(999.0f, 999.0f, 999.0f), s.GetMarble());
}

TEST(SceneFunctions, HideObjects)
{
	sf::Music m1;
	sf::Music m2;
	m1.openFromFile(level1_ogg);
	m2.openFromFile(level2_ogg);
	Scene s(&m1, &m2);

	s.HideObjects();

	EXPECT_EQ(Eigen::Vector3f(999.0f, 999.0f, 999.0f), s.GetMarble());
}

TEST(SceneFunctions, StartSingle)
{
	sf::Music m1;
	sf::Music m2;
	m1.openFromFile(level1_ogg);
	m2.openFromFile(level2_ogg);
	Scene s(&m1, &m2);

	s.StartSingle(7);

	EXPECT_EQ(true, s.IsSinglePlay());
	EXPECT_EQ(Scene::CamMode::ORBIT, s.GetMode());
	EXPECT_EQ(Eigen::Vector3f(999.0f, 999.0f, 999.0f), s.GetMarble());
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

	EXPECT_EQ(Scene::CamMode::DEORBIT, s.GetMode());
	EXPECT_NE(Eigen::Vector3f(999.0f, 999.0f, 999.0f), s.GetMarble());
}