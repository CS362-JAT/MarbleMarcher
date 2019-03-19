#pragma once

class Camera {

public:

	enum CamMode {
	    INTRO,
	    SCREEN_SAVER,
	    ORBIT,
	    DEORBIT,
	    MARBLE,
	    GOAL,
	    FINAL,
	    };
    
    Camera() : 
        cam_mat(Eigen::Matrix4f::Identity()),
        cam_look_x(0.0f),
        cam_look_y(0.0f),
        cam_dist(0.0f),
        cam_pos(0.0f, 0.0f, 0.0f),
        cam_mode(INTRO)
        {}

    void SetMatrix(const Eigen::Matrix4f& matrix) { cam_mat = matrix; }
    void SetLookX(float x) { cam_look_x = x; }
    void SetLookY(float y) { cam_look_y = y; }
    void SetDistance(float d) { cam_dist = d; }
    void SetPosition(const Eigen::Vector3f& pos) { cam_pos = pos; }
    void SetMode(CamMode mode) { cam_mode = mode; }
    void SetLookXSmooth(float x) { cam_look_x_smooth = x; }
    void SetLookYSmooth(float y) { cam_look_y_smooth = y; }
    void SetDistanceSmooth(float d) { cam_dist_smooth = d; }
    void SetPositionSmooth(const Eigen::Vector3f& pos) { cam_pos_smooth = pos; }

    Eigen::Matrix4f GetMatrix() const { return cam_mat; }
    float GetLookX() const { return cam_look_x; }
    float GetLookY() const { return cam_look_y; }
    float GetDistance() const { return cam_dist; }
    Eigen::Vector3f GetPosition() const { return cam_pos; }
    CamMode GetMode() const { return cam_mode; }
    float GetLookXSmooth() const { return cam_look_x_smooth; }
    float GetLookYSmooth() const { return cam_look_y_smooth; }
    float GetDistanceSmooth() const { return cam_dist_smooth; }
    Eigen::Vector3f GetPositionSmooth() const { return cam_pos_smooth; }

private:
    Eigen::Matrix4f cam_mat;
    float           cam_look_x;
    float           cam_look_y;
    float           cam_dist;
    Eigen::Vector3f cam_pos;
    CamMode         cam_mode;
    float           cam_look_x_smooth;
    float           cam_look_y_smooth;
    float           cam_dist_smooth;
    Eigen::Vector3f cam_pos_smooth;

};