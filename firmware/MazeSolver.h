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
	}
public:
	MoveAction *ma;
	WallDetector *wd;
	Maze maze, maze_backup;
	Agent agent;
	Thread thread;
	Agent::State prevState = Agent::IDLE;
	Direction dir;
	IndexVec pos;

	void start() {
		ma->enable();
		thread.start(this, &MazeSolver::task);
	}
	void robotMove(const Direction &nextDir) {
		if (dir == NORTH) {
			if (nextDir == NORTH) {
				ma->set_action(MoveAction::GO_STRAIGHT);
				pos.y++;
			} else if (nextDir == EAST) {
				ma->set_action(MoveAction::TURN_RIGHT_90);
				pos.x++;
			} else if (nextDir == SOUTH) {
				ma->set_action(MoveAction::RETURN);
				pos.y--;
			} else if (nextDir == WEST) {
				ma->set_action(MoveAction::TURN_LEFT_90);
				pos.x--;
			}
		} else if (dir == EAST) {
			if (nextDir == NORTH) {
				ma->set_action(MoveAction::TURN_LEFT_90);
				pos.y++;
			} else if (nextDir == EAST) {
				ma->set_action(MoveAction::GO_STRAIGHT);
				pos.x++;
			} else if (nextDir == SOUTH) {
				ma->set_action(MoveAction::TURN_RIGHT_90);
				pos.y--;
			} else if (nextDir == WEST) {
				ma->set_action(MoveAction::RETURN);
				pos.x--;
			}
		} else if (dir == SOUTH) {
			if (nextDir == NORTH) {
				ma->set_action(MoveAction::RETURN);
				pos.y++;
			} else if (nextDir == EAST) {
				ma->set_action(MoveAction::TURN_LEFT_90);
				pos.x++;
			} else if (nextDir == SOUTH) {
				ma->set_action(MoveAction::GO_STRAIGHT);
				pos.y--;
			} else if (nextDir == WEST) {
				ma->set_action(MoveAction::TURN_RIGHT_90);
				pos.x--;
			}
		} else if (dir == WEST) {
			if (nextDir == NORTH) {
				ma->set_action(MoveAction::TURN_RIGHT_90);
				pos.y++;
			} else if (nextDir == EAST) {
				ma->set_action(MoveAction::RETURN);
				pos.x++;
			} else if (nextDir == SOUTH) {
				ma->set_action(MoveAction::TURN_LEFT_90);
				pos.y--;
			} else if (nextDir == WEST) {
				ma->set_action(MoveAction::GO_STRAIGHT);
				pos.x--;
			}
		}
	}
	void robotMove(const Operation &op) {

	}
	void robotPositionInit() {

	}
	bool operationFinished() {
		return false;
	}
	Direction getWallData() {
		printf("%s %s %s %s\n", wd->wall().side[0] ? "X" : ".", wd->wall().flont[0] ? "X" : ".",
				wd->wall().flont[1] ? "X" : ".", wd->wall().side[1] ? "X" : ".");
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
		}
		wall |= DONE_NORTH;
		wall |= DONE_EAST;
		wall |= DONE_SOUTH;
		wall |= DONE_WEST;
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
			}

			Direction wallData = getWallData();		//< センサから取得した壁情報を入れる
			IndexVec robotPos = getRobotPosion();	//< ロボットの座標を取得

			agent.update(robotPos, wallData);		//< 壁情報を更新 次に進むべき方向を計算
			if (agent.getState() == Agent::FINISHED) break;	//Agentの状態を確認 FINISHEDになったら計測走行にうつる

			//ゴールにたどり着いた瞬間に一度だけmazeのバックアップをとる
			//Mazeクラスはoperator=が定義してあるからa = bでコピーできる
			if (prevState == Agent::SEARCHING_NOT_GOAL
					&& agent.getState() == Agent::SEARCHING_REACHED_GOAL) {
				maze_backup = maze;
			}
			prevState = agent.getState();

//			//一度はゴールにたどり着き、少なくともゴールできる状態で追加の探索をしているが、
//			//もう時間が無いから探索をうちやめてスタート地点に戻る
//			if (isTimeOut()
//					&& agent.getState() == Agent::SEARCHING_REACHED_GOAL) {
//				agent.forceGotoStart();
//			}

			Direction nextDir = agent.getNextDirection();		//< Agentの状態が探索中の場合は次に進むべき方向を取得する
			robotMove(nextDir);  //< robotMove関数はDirection型を受け取ってロボットをそっちに動かす関数
		}

		robotPositionInit();	//< ロボットを停止させ、スタートする向きに戻す

		//最短経路の計算 割と時間がかかる(数秒)
		//引数は斜め走行をするかしないか
		//trueだと斜め走行をする
		agent.calcRunSequence(false);

		/**********************************
		 * 計測走行
		 *********************************/
		//コマンドリストみたいなやつを取り出す
		const OperationList &runSequence = agent.getRunSequence();

		//Operationを先頭から順番に実行していく
		for (size_t i = 0; i < runSequence.size(); i++) {
			//Operationの実行が終わるまで待つ(nマス進んだ,右に曲がった)
			while (!operationFinished())
				;

			//i番目のを実行
			robotMove(runSequence[i]); //robotMode関数はOperation型を受け取ってそれを実行する関数
		}
	}
};

#endif /* MAZESOLVER_H_ */
