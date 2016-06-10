#include <pcl/point_types.h>

namespace TransformationUtils {
    // calcola il punto medio tra tutti i selezionati
    Eigen::Vector3f getCentroid(Eigen::MatrixXf M) {
        Eigen::Vector3f c = Eigen::Vector3f::Zero();
        for (int i = 0; i < M.cols(); i++) {
            c(0) += M(0, i);
            c(1) += M(1, i);
            c(2) += M(2, i);
        }
        c /= M.cols();
        return c;
    }

    // si elimina la parte di traslazione spostando le origini nei centroidi
    Eigen::MatrixXf centraMatrice(Eigen::MatrixXf M, Eigen::Vector3f c) {
        for (int i = 0; i < M.cols(); i++) {
            M(0, i) -= c(0);
            M(1, i) -= c(1);
            M(2, i) -= c(2);
        }
        return M;
    }

    Eigen::Matrix4f trovaT(Eigen::MatrixXf Mx, Eigen::MatrixXf My) {
        Eigen::Vector3f Cx = getCentroid(Mx);
        std::cout << " -> Centroide sinistro: " << "(" << Cx(0) << ", " << Cx(1) << ", " << Cx(2) << ")" << std::endl;
        Eigen::Vector3f Cy = getCentroid(My);
        std::cout << " -> Centroide destro: " << "(" << Cy(0) << ", " << Cy(1) << ", " << Cy(2) << ")" << std::endl;

        Eigen::MatrixXf X = centraMatrice(Mx, Cx);
        //std::cout << "Matrice X:" << std::endl << X << std::endl;
        Eigen::MatrixXf Y = centraMatrice(My, Cy);
        //std::cout << "Matrice Y:" << std::endl << Y << std::endl;

        Eigen::MatrixXf S = X * Y.transpose();
        //std::cout << "Matrice S:" << std::endl << S << std::endl;

        Eigen::JacobiSVD<Eigen::MatrixXf> svd(S, Eigen::ComputeFullU | Eigen::ComputeFullV);
        Eigen::MatrixXf U = svd.matrixU();
        Eigen::MatrixXf V = svd.matrixV();
        //std::cout << "Matrice U:" << std::endl << U << std::endl;
        //std::cout << "Matrice V:" << std::endl << V << std::endl;

        // scrivo una matrice identità con det(V*U.trasposto) come ultimo elemento
        Eigen::Matrix3f E = Eigen::Matrix<float, 3, 3>::Identity();
        E(2, 2) = (V*U.transpose()).determinant();

        // ricavo la rotazione e traslazione, componendo poi la matrice T
        Eigen::Matrix3f R = V*E*U.transpose();
        //std::cout << std::endl << "Matrice R:" << std::endl << R << std::endl;
        Eigen::Vector3f t = Cy - R*Cx;
        //std::cout << std::endl << "Vettore t:" << std::endl << t << std::endl;
        Eigen::Matrix4f T = Eigen::Matrix4f::Identity();
        T.block<3, 3>(0, 0) = R;
        T.rightCols<1>().block<3, 1>(0, 0) = t;
        std::cout << std::endl << "Matrice T:" << std::endl << T << std::endl;

        return T;
    }
}