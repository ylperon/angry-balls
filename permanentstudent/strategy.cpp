#include <iostream>
#include <vector>
#include <cmath>
#include <string>
#include <algorithm>
#include <map>
#include <set>
#include <math.h>
#include <iomanip>

const double PI = acos(-1.0);

class MovingStrategies {

private:
    struct CoinState
    {
        double X;
        double Y;

        CoinState() {
            X = 0;
            Y = 0;
        }

        CoinState(double curX, double curY) {
            X = curX;
            Y = curY;
        }
    };

    struct BallState
    {
        double X;
        double Y;
        double Vx;
        double Vy;
        double score;

        BallState() {
            X = 0;
            Y = 0;
            Vx = 0;
            Vy = 0;
            score = 0;
        }

        BallState(double curX, double curY, double curVx, double curVy, double curScore = 0) {
            X = curX;
            Y = curY;
            Vx = curVx;
            Vy = curVy;
            score = curScore;
        }
    };

    struct Acceleration
    {
        double Ax;
        double Ay;

        Acceleration() {
            Ax = 0;
            Ay = 0;
        }

        Acceleration(double curAx, double curAy) {
            Ax = curAx;
            Ay = curAy;
        }
    };

    struct FieldState
    {
        double fieldRadius;
        double ballRadius;
        double coinRadius;
        double timeQuant;
        double maxVelocity;

        std::vector<BallState> ballsStates;
        std::vector<CoinState> coinsStates;
    };

    struct BestMoveToCoin
    {
        double bestTime;
        Acceleration bestAcceleration;
        int coin_id;

        BestMoveToCoin(double time, const Acceleration& acceleration, int coinIndex) {
            bestTime = time;
            bestAcceleration = acceleration;
            coin_id = coinIndex;
        }
    };

    FieldState state;

    static const size_t MY_ID = 0;

    static const int ANGLE_STEP_NUMBER = 10000;

    BallState getNextState(BallState previousState, Acceleration acceleration) {
        double Vx_previous = previousState.Vx;
        double Vy_previous = previousState.Vy;
        double Vx_now = Vx_previous + acceleration.Ax * state.timeQuant;
        double Vy_now = Vy_previous + acceleration.Ay * state.timeQuant;
        if (Vx_now * Vx_now + Vy_now * Vy_now > state.maxVelocity * state.maxVelocity) {
            Vx_now /= sqrt(Vx_now * Vx_now + Vy_now * Vy_now);
            Vy_now /= sqrt(Vx_now * Vx_now + Vy_now * Vy_now);
            Vx_now *= state.maxVelocity;
            Vy_now *= state.maxVelocity;
        }
        double X_previous = previousState.X;
        double Y_previous = previousState.Y;
        double X_now = X_previous + Vx_now * state.timeQuant;
        double Y_now = Y_previous + Vy_now * state.timeQuant;
        if (sqrt(X_now * X_now + Y_now * Y_now) <= state.fieldRadius - state.ballRadius) {
            return BallState(X_now, Y_now, Vx_now, Vy_now);
        } else {

            double left_bound = 0, right_bound = 1;
            for (int iter = 0; iter < 50; ++iter) {
                double middle = (left_bound + right_bound) / 2;
                double X_mid = X_previous + Vx_now * middle * state.timeQuant;
                double Y_mid = Y_previous + Vy_now * middle * state.timeQuant;
                if (sqrt(X_mid * X_mid + Y_mid * Y_mid) <= state.fieldRadius - state.ballRadius) {
                    left_bound = middle;
                } else {
                    right_bound = middle;
                }
            }

            double X_mid = X_previous + Vx_now * left_bound * state.timeQuant;
            double Y_mid = Y_previous + Vy_now * left_bound * state.timeQuant;

            double Vabs = sqrt(Vx_now * Vx_now + Vy_now * Vy_now);
            double distToCenter = sqrt(X_mid * X_mid + Y_mid * Y_mid);
            double angle = asin((Vx_now * Y_mid - Vy_now * X_mid) / (Vabs * distToCenter));

            double angle_to_rotate = PI + 2 * angle;
            double Vx_next = Vx_now * cos(angle_to_rotate) - Vy_now * sin(angle_to_rotate);
            double Vy_next = Vx_now * sin(angle_to_rotate) + Vy_now * cos(angle_to_rotate);

            double X_now = X_mid + Vx_next * (1 - left_bound) * state.timeQuant;
            double Y_now = Y_mid + Vy_next * (1 - left_bound) * state.timeQuant;

            return BallState(X_now, Y_now, Vx_next, Vy_next);
        }
    }

    BestMoveToCoin getBestMove(size_t ballIndex) {
        std::vector<Acceleration> ballAccelerations(ANGLE_STEP_NUMBER);
        std::vector<BallState> ballStateNow(ANGLE_STEP_NUMBER), ballStateNext(ANGLE_STEP_NUMBER);

        for (size_t angleIndex = 0; angleIndex < ANGLE_STEP_NUMBER; ++angleIndex) {
            double curAngle = 2 * PI * angleIndex / ANGLE_STEP_NUMBER;
            ballAccelerations[angleIndex] = Acceleration(cos(curAngle), sin(curAngle));
            ballStateNow[angleIndex] = state.ballsStates[ballIndex];
        }

        for (size_t quantNumber = 1; quantNumber <= 100; ++quantNumber) {
            for (size_t angleIndex = 0; angleIndex < ANGLE_STEP_NUMBER; ++angleIndex) {
                ballStateNext[angleIndex] = getNextState(ballStateNow[angleIndex], ballAccelerations[angleIndex]);
                double X_now = ballStateNext[angleIndex].X;
                double Y_now = ballStateNext[angleIndex].Y;
                for (size_t coinIndex = 0; coinIndex < state.coinsStates.size(); ++coinIndex) {
                    double X_cur_coin = state.coinsStates[coinIndex].X;
                    double Y_cur_coin = state.coinsStates[coinIndex].Y;
                    double dist_to_coin = sqrt((X_now - X_cur_coin) * (X_now - X_cur_coin) + (Y_now - Y_cur_coin) * (Y_now - Y_cur_coin));
                    if (dist_to_coin <= state.ballRadius + state.coinRadius) {
                        return BestMoveToCoin(state.timeQuant * quantNumber, ballAccelerations[angleIndex], coinIndex);
                    }
                }
            }
            ballStateNow = ballStateNext;
        }

        return BestMoveToCoin(-1, Acceleration(), -1);
    }

public:
    MovingStrategies() {
        state.fieldRadius = 10;
        state.ballRadius = 1;
        state.coinRadius = 1;
        state.timeQuant = 0.1;
        state.maxVelocity = 10;
    }

    void setState(const FieldState& newState) {
        state = newState;
    }

    Acceleration movePredictiveSimple() {
        size_t ballNumber = state.ballsStates.size();
        std::vector<BestMoveToCoin> bestMoves;
        for (int ballIndex = 0; ballIndex < ballNumber; ++ballIndex) {
            bestMoves.push_back(getBestMove(ballIndex));
        }
        int aimCoin = bestMoves[MY_ID].coin_id;
        if (aimCoin == -1) {
            return Acceleration();
        }
        double bestTimeToCoin = bestMoves[MY_ID].bestTime;
        for (int ballIndex = 0; ballIndex < ballNumber; ++ballIndex) {
            if (bestMoves[ballIndex].coin_id == aimCoin && bestMoves[ballIndex].bestTime < bestTimeToCoin) {
                return Acceleration();
            }
        }
        return bestMoves[MY_ID].bestAcceleration;
    }

    void test() {
        state.fieldRadius = 10;
        state.ballRadius = 1;
        state.coinRadius = 1;
        state.timeQuant = 0.1;
        state.maxVelocity = 20;
        BallState firstBall(0, 0, 3, 5);
        BallState secondBall(4, 4, -3, 4);
        state.ballsStates.push_back(firstBall);
        state.ballsStates.push_back(secondBall);
        CoinState firstCoin(-7, 1);
        CoinState secondCoin(-3, -4);
        state.coinsStates.push_back(firstCoin);
        state.coinsStates.push_back(secondCoin);
        Acceleration bestAcceleration = movePredictiveSimple();
        double Ax = bestAcceleration.Ax;
        double Ay = bestAcceleration.Ay;
        printf("Ax = %.6lf, Ay = %.6lf\n",Ax,Ay);
    }
};


int main() {

    MovingStrategies strategies;
    strategies.test();
    return 0;
}