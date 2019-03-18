#pragma once

class Marble {
private:
    float           marble_rad;
    Eigen::Vector3f marble_pos;
    Eigen::Vector3f marble_vel;
    Eigen::Matrix3f marble_mat;

public:
    Marble() : 
        marble_pos(0.0f, 0.0f, 0.0f), 
        marble_vel(0.0f, 0.0f, 0.0f), 
        marble_mat(Eigen::Matrix3f::Identity()), 
        marble_rad(1.0f) {}

    void SetRadius(float r) { marble_rad = r; }
    void SetPosition(const Eigen::Vector3f& pos) { marble_pos = pos; }
    void SetVelocity(const Eigen::Vector3f& vel) { marble_vel = vel; }
    void SetMatrix(const Eigen::Matrix3f& matrix) { marble_mat = matrix; }

    float GetRadius() const { return marble_rad; }
    Eigen::Vector3f GetPosition() const { return marble_pos; }
    Eigen::Vector3f GetVelocity() const { return marble_vel; }
    Eigen::Matrix3f GetMatrix() const { return marble_mat; }
};