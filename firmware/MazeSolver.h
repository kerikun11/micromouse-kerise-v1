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
	MazeSolver(MoveAction *ma) :
			ma(ma), agent(maze), thread(PRIORITY_MAZE_SOLVER) {
	}
public:
	MoveAction *ma;
	Maze maze, maze_backup;
	Agent agent;
	Thread thread;
	Agent::State prevState = Agent::IDLE;
	IndexVec pos;

	void start() {
		thread.start(this, &MazeSolver::task);
	}
	void robotMove(const Direction &dir) {

	}
	void robotMove(const Operation &op) {

	}
	void robotPositionInit() {

	}
	bool operationFinished() {
		return false;
	}
	Direction getWallData() {
		Direction dir;
		return dir;
	}
	IndexVec getRobotPosion() {
		return pos;
	}
	void task() {
		while (1) {
			Thread::signal_wait(0x01);

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

			Direction nextDir = agent.getNextDirection();//< Agentの状態が探索中の場合は次に進むべき方向を取得する
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
