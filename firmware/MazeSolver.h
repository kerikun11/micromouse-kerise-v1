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
	MazeSolver(Buzzer *bz, MoveAction *ma, WallDetector *wd) :
			bz(bz), ma(ma), wd(wd), agent(maze), thread(PRIORITY_MAZE_SOLVER, 4096) {
		dir = NORTH;
		pos = IndexVec(0, 0);
	}
	void start() {
		ma->enable();
		thread.start(this, &MazeSolver::task);
	}
private:
	Buzzer *bz;
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
		for (int i = 0; i < op.n; i++)
			switch (op.op) {
				case Operation::FORWARD:
					ma->set_action(MoveAction::FAST_GO_STRAIGHT);
					break;
				case Operation::FORWARD_DIAG:
//					ma->set_action(MoveAction::FAST_GO_DIAGONAL);
					break;
				case Operation::TURN_LEFT90:
					ma->set_action(MoveAction::FAST_TURN_LEFT_90);
					break;
				case Operation::TURN_LEFT45:
//					ma->set_action(MoveAction::FAST_TURN_LEFT_45);
					break;
				case Operation::TURN_RIGHT90:
					ma->set_action(MoveAction::FAST_TURN_RIGHT_90);
					break;
				case Operation::TURN_RIGHT45:
//					ma->set_action(MoveAction::FAST_TURN_RIGHT_45);
					break;
				case Operation::STOP:
					ma->set_action(MoveAction::FAST_STOP);
					break;
			}
	}
	void robotPositionInit() {
		ma->set_action(MoveAction::START_INIT);
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
	void task() {
		ma->set_action(MoveAction::START_STEP);
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
			if (nextDir == 0) {
				bz->play(Buzzer::BUZZER_MUSIC_ERROR);
				while (1) {
					Thread::wait(100);
				}
			}
			robotMove(nextDir);  //< robotMove関数はDirection型を受け取ってロボットをそっちに動かす関数
		}
		maze_backup = maze;

		printf("robotPositionInit();\n");
		robotPositionInit();	//< ロボットを停止させ、スタートする向きに戻す
		bz->play(Buzzer::BUZZER_MUSIC_SELECT);

		printf("maze.printWall();\n");
		maze.printWall();

		Thread::wait(3000);

		//最短経路の計算 割と時間がかかる(数秒)
		//引数は斜め走行をするかしないか
		//trueだと斜め走行をする
		printf("agent.calcRunSequence();\n");
		agent.calcRunSequence(false);

		/**********************************
		 * 計測走行
		 *********************************/
		//コマンドリストみたいなやつを取り出す
		printf("agent.getRunSequence();\n");
		const OperationList &runSequence = agent.getRunSequence();

		printf("runSequence.size() => %d\n", runSequence.size());

		bz->play(Buzzer::BUZZER_MUSIC_CONFIRM);
		//Operationを先頭から順番に実行していく
		for (size_t i = 0; i < runSequence.size(); i++) {
			printf("runSequence[%d].n => %d, runSequence[%d].op => %d\n", i, runSequence[i].n, i,
					runSequence[i].op);
			if (i == 0) {
				ma->set_action(MoveAction::FAST_START_STEP);
				for (int j = 0; j < runSequence[i].n - 1; j++) {
					ma->set_action(MoveAction::FAST_GO_STRAIGHT);
				}
			} else {
				robotMove(runSequence[i]); //robotMode関数はOperation型を受け取ってそれを実行する関数
			}
			Thread::wait(1);
		}
		ma->set_action(MoveAction::FAST_STOP);

		while (ma->actions()) {
			Thread::wait(1);
		}
		bz->play(Buzzer::BUZZER_MUSIC_COMPLETE);

//		printf("End.\n");
//		Maze field;
//		Maze mazeInRobot;
//		field.loadFromArray(mazeData_66test);
//
//		Agent agent(mazeInRobot);
//
//		IndexVec cur(0, 0);
//		while (1) {
//			bool pos[MAZE_SIZE][MAZE_SIZE] = { false };
//			pos[cur.y][cur.x] = true;
//			mazeInRobot.printWall(pos);
//
//			agent.update(cur, field.getWall(cur));
//			if (agent.getState() == Agent::FINISHED) break;
//
//			Direction dir = agent.getNextDirection();
//			for (int i = 0; i < 4; i++) {
//				if (dir[i]) cur += IndexVec::vecDir[i];
//			}
//			Thread::wait(100);
//		}
//
//		agent.calcRunSequence(false);
//		const OperationList &runSequence = agent.getRunSequence();
//		printf("runSequence.size() => %d\n", runSequence.size());
//
//		bool route[MAZE_SIZE][MAZE_SIZE] = { false };
//		for (auto &index : agent.getShortestPath()) {
//			route[index.y][index.x] = true;
//		}
//		mazeInRobot.printWall(route);
	}
};

#endif /* MAZESOLVER_H_ */
