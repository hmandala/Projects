#include <iostream>
#include "config.hpp"
#include <vector>
using Eigen::MatrixXd;
using Eigen::VectorXd;
using namespace std;

void WorkspaceController() // this should get 2 points in R^3
{
}

void MakeT(MatrixXd &T, double angle, int axis, double a, double b, double c){
    T.setIdentity();
 
    if (axis == 0) {
        T(1,1) = cos(angle);
        T(1,2) = -1 * sin(angle);
        T(2,1) = sin(angle);
        T(2,2) = cos(angle);
    }
    else if (axis == 1) {
        T(0,0) = cos(angle);
        T(0,2) = -1 * sin(angle);
        T(2,0) = sin(angle);
        T(2,2) = cos(angle);
    }
    else if (axis == 2) {
        T(0,0) = cos(angle);
        T(0,1) = -1 * sin(angle);
        T(1,0) = sin(angle);
        T(1,1) = cos(angle);
    }
   
    T(0,3) = a;
    T(1,3) = b;
    T(2,3) = c;
}

void findvsp(MatrixXd &v, VectorXd &s, MatrixXd &p, VectorXd theta, VectorXd l){
    vector<MatrixXd> T;
    MatrixXd temp_r(4,4);
    MatrixXd temp_t(4,4);
    MatrixXd temp(4,4);
    MatrixXd temp_for_s(4,4);
    temp.setIdentity();
    temp_for_s.setIdentity();

    MatrixXd Twb(4,4);
    MatrixXd Tb1(4,4);
    MakeT(Twb, 0.0, 2, 0.0,0.0,0.0);
    MakeT(Tb1, 0.0, 2, 0.0,0.0,0.0);
    temp_t = Twb * Tb1;

    MatrixXd rot(3,3);
    MatrixXd eye(3,3);
    eye.setIdentity();
    for (int i = 0; i < 7; i++) {
        int axis = 0;
        if (v(0, i) == 1)
            axis = 0;
        else if (v(1, i) == 1 )
            axis = 1;
        else if (v(2, i) == 1)
            axis = 2;
        MakeT(temp_r, theta[i], axis, 0, 0, 0);
        T.push_back(temp_r);
        MakeT(temp_t, 0, axis, 0, l[i], 0);
        T.push_back(temp_t);
        temp= temp_t * temp_r;
        temp_for_s *= temp;
        p(0, i) = temp_for_s(0,3);
        p(1, i) = temp_for_s(1,3);
        p(2, i) = temp_for_s(2,3);
        //cout << "temp_for_s:" << temp_for_s << endl;
        /*
        for (int j = 0; j < 3; j++){
            for (int k = 0; k < 3; k++){
                rot(j,k) = temp_for_s(j,k);
            }
        }
        rot -= eye;
        VectorXd y(3);
        //cout << "rot: " << rot << endl;   
        y << 0,0,0;
        //v.col(i) = rot.fullPivLu().solve(y);       
        */
    }
   
    s(0) = temp_for_s(0,3);
    s(1) = temp_for_s(1,3);
    s(2) = temp_for_s(2,3);
   
    //cout << "v: " << v << endl;
}

void findJ(MatrixXd &J, MatrixXd v, MatrixXd s, MatrixXd p){
    for (int j = 0; j < J.cols(); j++) {
        Eigen::Vector3d d = s.col(0) - p.col(j);
        J(0,j) = v(1,j) * d(2) - d(1) * v(2,j);
        J(1,j) = v(2,j) * d(0) - d(2) * v(0,j);
        J(2,j) = v(0,j) * d(1) - d(0) * v(1,j);   
    }
}

void findPSI(MatrixXd &J_I, MatrixXd &J){
    Eigen::JacobiSVD<MatrixXd> svd(J, Eigen::ComputeFullU|Eigen::ComputeFullV);
    VectorXd s = svd.singularValues();
    MatrixXd U = svd.matrixU();
    MatrixXd V = svd.matrixV();
 
    MatrixXd S(s.rows(), V.rows());
    for (int i = 0; i < s.rows(); i++) {
        S(i,i) = s(i);
    }
    J_I = V * (S.transpose() * U.transpose());
}

void findDeltaTheta(VectorXd &delta_theta, MatrixXd &J, MatrixXd &J_I, VectorXd ef_g, VectorXd ef_c){
    findPSI(J_I, J); 
    VectorXd delta_e = ef_g - ef_c;
    double error = 999999;
    MatrixXd eye(J.rows(), J_I.cols());
    eye.setIdentity();   
    while (error > TOLERANCE) {   
        VectorXd err = (eye - (J*J_I)) * delta_e;   
        error = err.squaredNorm();
        delta_e /= 2;    
    }
    delta_theta = J_I * delta_e;
}

void JacobianIK(VectorXd ef_g, VectorXd theta, VectorXd l){
    int iter = 0;  // init number of iterations undergone to 0
    MatrixXd J(3,7); // Jacobian matrix
    MatrixXd J_I(7,3); // Jacobian pseudoinverse matrix
    VectorXd delta_theta(7); // the change in theta which is to be calculated at each iteration
    // TODO : Init all these 3:
    MatrixXd v(3,7); // unit vectors parallel to axis of rotation for each joint
    VectorXd s(3); // current end effector position
    MatrixXd p(3,7); // current Joint positions
    p << 0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1; // init with some junk values.
    s << 0,0,0; // junk values
    v << 1,0,0,1,0,0,1,
         0,1,0,0,1,0,0,
             0,0,1,0,0,1,0;  // NOT junk values - these signify rotation in the Z axis
    findvsp(v,s,p, theta, l); // init
    VectorXd diff_temp(3);
    diff_temp << 99999.0,99999.0,99999.0;   
    diff_temp = s - ef_g;

    cout << "Starting End Effector position: " << endl << s << endl;
    while(diff_temp.squaredNorm() > EF_ERROR_TOLERANCE && iter <  MAX_ITER) {
        findJ(J, v, s, p); // this populates J.
        findDeltaTheta(delta_theta, J, J_I, ef_g, s); // finds the delta thetas for each joint angles needed to get to the goal
        theta = theta + delta_theta; // update value of theta
        findvsp(v, s, p, theta, l); // update v
        diff_temp = s - ef_g;
        //cout << "End Effector Position: " << endl << s << endl;
    }
    cout << "Final End Effector Position: " << endl << s << endl;
    std::cout << "Joint Angles:" << theta << endl;
}

int main(){
    // joint angles
    VectorXd q(7);
    q << -0.3,-0.4, -0.5, -0.4, -0.3, -0.25, -0.4;
    //q << 0.0,0.0, 0.0, 0.0, 0.0, 0.0, 0.0;
    // Link Lengths
    VectorXd l(8);
    l << 0.2, 0.2, 0.2, 0.2, 0.2, 0.2, 0.2, 0.2;

    //goal end effector position
    VectorXd ef_g(3);
    ef_g << 0.7,0.3,-0.3;

    cout << "Calling JacobianIK" << endl;
    JacobianIK(ef_g, q, l); // this will call the main looping subprogram
    cout << "Success! Exiting." << endl;
}
