/*
 * MazeSolver.h
 *
 *  Created on: 2016/11/04
 *      Author: kerikun11
 */

#ifndef MAZESOLVER_H_
#define MAZESOLVER_H_

#include "mbed.h"
#include "config.h"
#include "SpeedController.h"
#include "MazeSolver.h"
#include "MazeSolver/Agent.h"
#include "MazeSolver/mazeData.h"
#include <vector>

class MazeSolver {
public:
	MazeSolver(Buzzer *bz, MPU6500 *mpu, Reflector *rfl, MoveAction *ma, WallDetector *wd) :
			bz(bz), mpu(mpu), rfl(rfl), ma(ma), wd(wd), agent(maze),
					thread(PRIORITY_MAZE_SOLVER, STACK_SIZE_MAZE_SOLVER) {
		dir = NORTH;
		pos = IndexVec(0, 0);
	}
	void start() {
		thread.start(this, &MazeSolver::task);
		printf("0x%08X: Maze Solver\n", (unsigned int) thread.gettid());
	}
	void terminate() {
		thread.terminate();
		ma->disable();
	}
private:
	Buzzer *bz;
	MPU6500 *mpu;
	Reflector *rfl;
	MoveAction *ma;
	WallDetector *wd;
	Maze maze, maze_backup;
	Agent agent;
	Thread thread;
	Agent::State prevState = Agent::IDLE;
	Direction dir;
	IndexVec pos;

	void robotMove(const Direction &nextDir) {
		if (dir == NORTH) {
			if (nextDir == NORTH) {
				ma->set_action(MoveAction::GO_STRAIGHT);
				pos.y++;
				dir = NORTH;
			} else if (nextDir == EAST) {
				ma->set_action(MoveAction::TURN_RIGHT_90);
				pos.x++;
				dir = EAST;
			} else if (nextDir == SOUTH) {
				ma->set_action(MoveAction::RETURN);
				ma->set_action(MoveAction::GO_STRAIGHT);
				pos.y--;
				dir = SOUTH;
			} else if (nextDir == WEST) {
				ma->set_action(MoveAction::TURN_LEFT_90);
				pos.x--;
				dir = WEST;
			}
		} else if (dir == EAST) {
			if (nextDir == NORTH) {
				ma->set_action(MoveAction::TURN_LEFT_90);
				pos.y++;
				dir = NORTH;
			} else if (nextDir == EAST) {
				ma->set_action(MoveAction::GO_STRAIGHT);
				pos.x++;
				dir = EAST;
			} else if (nextDir == SOUTH) {
				ma->set_action(MoveAction::TURN_RIGHT_90);
				pos.y--;
				dir = SOUTH;
			} else if (nextDir == WEST) {
				ma->set_action(MoveAction::RETURN);
				ma->set_action(MoveAction::GO_STRAIGHT);
				pos.x--;
				dir = WEST;
			}
		} else if (dir == SOUTH) {
			if (nextDir == NORTH) {
				ma->set_action(MoveAction::RETURN);
				ma->set_action(MoveAction::GO_STRAIGHT);
				pos.y++;
				dir = NORTH;
			} else if (nextDir == EAST) {
				ma->set_action(MoveAction::TURN_LEFT_90);
				pos.x++;
				dir = EAST;
			} else if (nextDir == SOUTH) {
				ma->set_action(MoveAction::GO_STRAIGHT);
				pos.y--;
				dir = SOUTH;
			} else if (nextDir == WEST) {
				ma->set_action(MoveAction::TURN_RIGHT_90);
				pos.x--;
				dir = WEST;
			}
		} else if (dir == WEST) {
			if (nextDir == NORTH) {
				ma->set_action(MoveAction::TURN_RIGHT_90);
				pos.y++;
				dir = NORTH;
			} else if (nextDir == EAST) {
				ma->set_action(MoveAction::RETURN);
				ma->set_action(MoveAction::GO_STRAIGHT);
				pos.x++;
				dir = EAST;
			} else if (nextDir == SOUTH) {
				ma->set_action(MoveAction::TURN_LEFT_90);
				pos.y--;
				dir = SOUTH;
			} else if (nextDir == WEST) {
				ma->set_action(MoveAction::GO_STRAIGHT);
				pos.x--;
				dir = WEST;
			}
		}
	}
	void robotMove(const Operation &op, bool back = false) {
		switch (op.op) {
			case Operation::FORWARD:
				ma->set_action(MoveAction::FAST_GO_STRAIGHT);
				break;
			case Operation::FORWARD_DIAG:
//					ma->set_action(MoveAction::FAST_GO_DIAGONAL);
				break;
			case Operation::TURN_LEFT90:
				if (back) ma->set_action(MoveAction::FAST_TURN_RIGHT_90);
				else ma->set_action(MoveAction::FAST_TURN_LEFT_90);
				break;
			case Operation::TURN_LEFT45:
//					ma->set_action(MoveAction::FAST_TURN_LEFT_45);
				break;
			case Operation::TURN_RIGHT90:
				if (back) ma->set_action(MoveAction::FAST_TURN_LEFT_90);
				else ma->set_action(MoveAction::FAST_TURN_RIGHT_90);
				break;
			case Operation::TURN_RIGHT45:
//					ma->set_action(MoveAction::FAST_TURN_RIGHT_45);
				break;
			case Operation::STOP:
				ma->set_action(MoveAction::FAST_STOP);
				break;
		}
	}
//	void robotMove(const Operation &op) {
//		for (int i = 0; i < op.n; i++)
//			switch (op.op) {
//				case Operation::FORWARD:
//					ma->set_action(MoveAction::FAST_GO_STRAIGHT);
//					break;
//				case Operation::FORWARD_DIAG:
//					ma->set_action(MoveAction::FAST_GO_DIAGONAL);
//					break;
//				case Operation::TURN_LEFT90:
//					ma->set_action(MoveAction::FAST_TURN_LEFT_90);
//					break;
//				case Operation::TURN_LEFT45:
//					ma->set_action(MoveAction::FAST_TURN_LEFT_45);
//					break;
//				case Operation::TURN_RIGHT90:
//					ma->set_action(MoveAction::FAST_TURN_RIGHT_90);
//					break;
//				case Operation::TURN_RIGHT45:
//					ma->set_action(MoveAction::FAST_TURN_RIGHT_45);
//					break;
//				case Operation::STOP:
//					ma->set_action(MoveAction::FAST_STOP);
//					break;
//			}
//	}
	Direction getWallData() {
		Direction wall;
		if (dir == NORTH) {
			if (wd->wall().side[0]) {
				wall |= WEST;
			}
			if (wd->wall().side[1]) {
				wall |= EAST;
			}
			if (wd->wall().flont[0] && wd->wall().flont[1]) {
				wall |= NORTH;
			}
			wall |= DONE_NORTH;
			wall |= DONE_EAST;
			wall |= DONE_SOUTH;
			wall |= DONE_WEST;
		} else if (dir == EAST) {
			if (wd->wall().side[0]) {
				wall |= NORTH;
			}
			if (wd->wall().side[1]) {
				wall |= SOUTH;
			}
			if (wd->wall().flont[0] && wd->wall().flont[1]) {
				wall |= EAST;
			}
			wall |= DONE_NORTH;
			wall |= DONE_EAST;
			wall |= DONE_SOUTH;
			wall |= DONE_WEST;
		} else if (dir == SOUTH) {
			if (wd->wall().side[0]) {
				wall |= EAST;
			}
			if (wd->wall().side[1]) {
				wall |= WEST;
			}
			if (wd->wall().flont[0] && wd->wall().flont[1]) {
				wall |= SOUTH;
			}
			wall |= DONE_NORTH;
			wall |= DONE_EAST;
			wall |= DONE_SOUTH;
			wall |= DONE_WEST;
		} else if (dir == WEST) {
			if (wd->wall().side[0]) {
				wall |= SOUTH;
			}
			if (wd->wall().side[1]) {
				wall |= NORTH;
			}
			if (wd->wall().flont[0] && wd->wall().flont[1]) {
				wall |= WEST;
			}
			wall |= DONE_NORTH;
			wall |= DONE_EAST;
			wall |= DONE_SOUTH;
			wall |= DONE_WEST;
		}
		return wall;
	}
	IndexVec getRobotPosion() {
		return pos;
	}
	void search_run() {
		if (agent.getState() == Agent::FINISHED || agent.getState() == Agent::BACK_TO_START) return; //Agentの状態を確認 FINISHEDになったら計測走行にうつる
		dir = NORTH;
		pos = IndexVec(0, 0);
		maze = maze_backup;
		ma->set_action(MoveAction::START_STEP);
		mpu->calibration();
		wd->calibration();
		ma->enable();
		while (1) {
			while (ma->actions()) {
				Thread::wait(1);
			}

			Direction wallData = getWallData();		//< センサから取得した壁情報を入れる
			IndexVec robotPos = getRobotPosion();	//< ロボットの座標を取得
			printf("Position:\t(%d, %d)\tWall:\t%X\n", (int) robotPos.x, (int) robotPos.y,
					(int) wallData);

			agent.update(robotPos, wallData);		//< 壁情報を更新 次に進むべき方向を計算
			printf("agent.getState() => %d\n", agent.getState());
			if (agent.getState() == Agent::FINISHED) break;	//Agentの状態を確認 FINISHEDになったら計測走行にうつる

			//ゴールにたどり着いた瞬間に一度だけmazeのバックアップをとる
			//Mazeクラスはoperator=が定義してあるからa = bでコピーできる
			if (prevState == Agent::SEARCHING_NOT_GOAL
					&& agent.getState() == Agent::SEARCHING_REACHED_GOAL) {
				printf("maze_backup\n");
				maze_backup = maze;
				bz->play(Buzzer::CONFIRM);
			}
			prevState = agent.getState();

			Direction nextDir = agent.getNextDirection();		//< Agentの状態が探索中の場合は次に進むべき方向を取得する
			printf("agent.getNextDirection() => %X\n", (int) nextDir);
			if (nextDir == 0) {
				bz->play(Buzzer::ERROR);
				while (1) {
					Thread::wait(100);
				}
			}
			robotMove(nextDir);  //< robotMove関数はDirection型を受け取ってロボットをそっちに動かす関数
		}
		maze_backup = maze;
		ma->set_action(MoveAction::START_INIT);
		while (ma->actions()) {
			Thread::wait(1);
		}
		ma->disable();
		bz->play(Buzzer::COMPLETE);

		maze.printWall();
		Thread::wait(10);

		printf("agent.calcRunSequence();\n");
		agent.calcRunSequence(false);
	}
	void fast_run() {
		const OperationList &runSequence = agent.getRunSequence();
		printf("runSequence.size() => %d\n", runSequence.size());
		bz->play(Buzzer::CONFIRM);

		dir = NORTH;
		pos = IndexVec(0, 0);

		ma->set_action(MoveAction::FAST_START_STEP);
		for (size_t i = 0; i < runSequence.size(); i++) {
			printf("runSequence[%d].n => %d, runSequence[%d].op => %d\n", i, runSequence[i].n, i,
					runSequence[i].op);
			const Operation& op = runSequence[i];
			if (i == 0) {
				for (int j = 0; j < op.n - 1; j++) {
					robotMove(op);
				}
			} else {
				for (int j = 0; j < op.n; j++) {
					robotMove(op);
				}
			}
			Thread::wait(1);
		}
		ma->set_action(MoveAction::FAST_STOP);

		// start drive
		mpu->calibration();
//		wd->calibration();
		ma->enable();
		while (ma->actions()) {
			Thread::wait(1);
		}
		bz->play(Buzzer::COMPLETE);
		// end drive

		// back to start
		printf("Back to Start\n");
		ma->set_action(MoveAction::RETURN);
		ma->set_action(MoveAction::FAST_GO_HALF);
		for (size_t i = 0; i < runSequence.size(); i++) {
			printf("runSequence[%d].n => %d, runSequence[%d].op => %d\n",
					runSequence.size() - i - 1, runSequence[runSequence.size() - 1 - i].n,
					runSequence.size() - 1 - i, runSequence[runSequence.size() - 1 - i].op);
			const Operation& op = runSequence[runSequence.size() - 1 - i];
			if (i == runSequence.size() - 1) {
				for (int j = 0; j < op.n - 1; j++) {
					robotMove(op, true);
				}
			} else {
				for (int j = 0; j < op.n; j++) {
					robotMove(op, true);
				}
			}
			Thread::wait(1);
		}
		ma->set_action(MoveAction::FAST_STOP);

		ma->set_action(MoveAction::START_INIT);
		while (ma->actions()) {
			Thread::wait(1);
		}

		ma->disable();
		bz->play(Buzzer::COMPLETE);
	}
	void task() {
		search_run();
		Thread::wait(1000);
		while (1) {
			fast_run();
			Thread::wait(1000);
			ma->set_params_relative(200);
		}
	}
};

#endif /* MAZESOLVER_H_ */
