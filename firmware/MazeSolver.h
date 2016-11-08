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
#include "Controller.h"
#include "MazeSolver.h"
#include "MazeSolver/Agent.h"
#include "MazeSolver/mazeData.h"
#include <vector>

class MazeSolver {
public:
	MazeSolver(MoveAction *ma, WallDetector *wd) :
			ma(ma), wd(wd), agent(maze), thread(PRIORITY_MAZE_SOLVER) {
		dir = NORTH;
		pos = IndexVec(0, 0);
	}
	void start() {
		ma->enable();
		thread.start(this, &MazeSolver::task);
	}
private:
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
				pos.x--;
				dir = WEST;
			}
		} else if (dir == SOUTH) {
			if (nextDir == NORTH) {
				ma->set_action(MoveAction::RETURN);
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
	void robotMove(const Operation &op) {
		for (int i = 0; i < op.n; i++) {
			switch (op.op) {
				case Operation::FORWARD:
					ma->set_action(MoveAction::GO_STRAIGHT);
					break;
				case Operation::FORWARD_DIAG:
					break;
				case Operation::TURN_LEFT90:
					ma->set_action(MoveAction::TURN_LEFT_90);
					break;
				case Operation::TURN_LEFT45:
					break;
				case Operation::TURN_RIGHT90:
					ma->set_action(MoveAction::TURN_RIGHT_90);
					break;
				case Operation::TURN_RIGHT45:
					break;
				case Operation::STOP:
					ma->set_action(MoveAction::START_INIT);
					break;
			}
		}
	}
	void robotPositionInit() {
		ma->set_action(MoveAction::START_INIT);
	}
	bool operationFinished() {
		return false;
	}
	Direction getWallData() {
		Direction wall;
		if (dir == NORTH) {
			if (wd->wall().side[0]) {
				wall |= WEST;
			}
			if (wd->wall().side[1]) {
				wall |= EAST;
			}
			if (wd->wall().flont_flont[0] && wd->wall().flont_flont[1]) {
				wall |= NORTH;
				wall |= DONE_NORTH;
			} else if (!wd->wall().flont_flont[0] && !wd->wall().flont_flont[1]) {
				wall |= DONE_NORTH;
			} else {
				if (wd->wall().side[0] && wd->wall().side[1]) {
					wall |= DONE_NORTH;
				}
			}
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
			if (wd->wall().flont_flont[0] && wd->wall().flont_flont[1]) {
				wall |= EAST;
				wall |= DONE_EAST;
			} else if (!wd->wall().flont_flont[0] && !wd->wall().flont_flont[1]) {
				wall |= DONE_EAST;
			} else {
				if (wd->wall().side[0] && wd->wall().side[1]) {
					wall |= DONE_EAST;
				}
			}
			wall |= DONE_NORTH;
			wall |= DONE_SOUTH;
			wall |= DONE_WEST;
		} else if (dir == SOUTH) {
			if (wd->wall().side[0]) {
				wall |= EAST;
			}
			if (wd->wall().side[1]) {
				wall |= WEST;
			}
			if (wd->wall().flont_flont[0] && wd->wall().flont_flont[1]) {
				wall |= SOUTH;
				wall |= DONE_SOUTH;
			} else if (!wd->wall().flont_flont[0] && !wd->wall().flont_flont[1]) {
				wall |= DONE_SOUTH;
			} else {
				if (wd->wall().side[0] && wd->wall().side[1]) {
					wall |= DONE_SOUTH;
				}
			}
			wall |= DONE_NORTH;
			wall |= DONE_EAST;
			wall |= DONE_WEST;
		} else if (dir == WEST) {
			if (wd->wall().side[0]) {
				wall |= SOUTH;
			}
			if (wd->wall().side[1]) {
				wall |= NORTH;
			}
			if (wd->wall().flont_flont[0] && wd->wall().flont_flont[1]) {
				wall |= WEST;
				wall |= DONE_WEST;
			} else if (!wd->wall().flont_flont[0] && !wd->wall().flont_flont[1]) {
				wall |= DONE_WEST;
			} else {
				if (wd->wall().side[0] && wd->wall().side[1]) {
					wall |= DONE_WEST;
				}
			}
			wall |= DONE_NORTH;
			wall |= DONE_EAST;
			wall |= DONE_SOUTH;
		}
		return wall;
	}
	IndexVec getRobotPosion() {
		return pos;
	}
	void task() {
		ma->set_action(MoveAction::START_STEP);
		pos.y++;
		while (1) {
			while (ma->actions()) {
				Thread::wait(1);
			}

			printf("getWallData() => %x\n", (int) getWallData());
			Direction wallData = getWallData();		//< センサから取得した壁情報を入れる
			printf("getRobotPosition() => (%d,%d)\n", (int) getRobotPosion().x,
					(int) getRobotPosion().y);
			IndexVec robotPos = getRobotPosion();	//< ロボットの座標を取得

			printf("agent.update();\n");
			agent.update(robotPos, wallData);		//< 壁情報を更新 次に進むべき方向を計算
			printf("agent.getState() => %d\n", agent.getState());
			if (agent.getState() == Agent::FINISHED) break;	//Agentの状態を確認 FINISHEDになったら計測走行にうつる

			//ゴールにたどり着いた瞬間に一度だけmazeのバックアップをとる
			//Mazeクラスはoperator=が定義してあるからa = bでコピーできる
			if (prevState == Agent::SEARCHING_NOT_GOAL
					&& agent.getState() == Agent::SEARCHING_REACHED_GOAL) {
				printf("maze_backup\n");
				maze_backup = maze;
			}
			prevState = agent.getState();

			Direction nextDir = agent.getNextDirection();		//< Agentの状態が探索中の場合は次に進むべき方向を取得する
			printf("agent.getNextDirection() => %x\n", (int) nextDir);
			robotMove(nextDir);  //< robotMove関数はDirection型を受け取ってロボットをそっちに動かす関数
		}

		printf("robotPositionInit();");
		robotPositionInit();	//< ロボットを停止させ、スタートする向きに戻す

		//最短経路の計算 割と時間がかかる(数秒)
		//引数は斜め走行をするかしないか
		//trueだと斜め走行をする
		printf("agent.calcRunSequence(false);\n");
		agent.calcRunSequence(false);

		/**********************************
		 * 計測走行
		 *********************************/
		//コマンドリストみたいなやつを取り出す
		printf("agent.getRunSequence()\n");
		const OperationList &runSequence = agent.getRunSequence();

		//Operationを先頭から順番に実行していく
		for (size_t i = 0; i < runSequence.size(); i++) {
			//Operationの実行が終わるまで待つ(nマス進んだ,右に曲がった)
			while (!operationFinished())
				;

			//i番目のを実行
			robotMove(runSequence[i]); //robotMode関数はOperation型を受け取ってそれを実行する関数
		}
		printf("End.\n");
	}
};

#endif /* MAZESOLVER_H_ */
