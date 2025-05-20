//*********************************************************  
// Program: main.cpp  
// Course: CCP6124 OOPDS 
// Lecture Class: TC2L
// Tutorial Class: TT5L
// Trimester: 2510
// Member_1: 242UC244JT | CHONG MENG HANG | chong.meng.hang@student.mmu.edu.my | 016-8711296
// Member_2: 242UC244PF | TAN YONG XIN | tan.yong.xin@student.mmu.edu.my | 012-6556505
// Member_3: 242UC244S6 | THAM MEI TING | tham.mei.ting@student.mmu.edu.my | 017-3268006
// Member_4: 242UC244KX | YAP CHI YI | yap.chi.yi@student.mmu.edu.my | 018-2694514	 
//*********************************************************
// Task Distribution
// Member_1: CHONG MENG HANG
// Member_2: TAN YONG XIN
// Member_3: THAM MEI TING 
// Member_4: YAP CHI YI
// ********************************************************

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <vector>
#include <queue>
#include <cstdlib>
#include <ctime>

using namespace std;


class Battlefield;

class Robot {
protected:
    int robotPosX = -1;
    int robotPosY = -1;

    string id_ = ""; // id to be displayed on the battlefield
    string robotType_ = "";
    string robotName_ = ""; // id_robotName eg GR05_Star

    int numOfLives_ = 3; // number of lives
    int numOfKills_ = 0; // number of kills

public:
    //PC + DC
    Robot(string id = "", int x = -1, int y = -1) : id_(id), robotPosX(x), robotPosY(y) {}

    virtual ~Robot() {}

    //Getter and Setter Functions

    int x() const { return robotPosX; } // get x position of robot
    void setX(int x) { robotPosX = x; } // set x position of robot

    int y() const { return robotPosY; } // get y position of robot
    void setY(int y) { robotPosY = y; } // set y position of robot

    string id() const { return id_; } // get id of robot
    void setId(string id) { id_ = id; } // set id of robot

    string robotType() const { return robotType_; } // get robot type
    void setRobotType(string robotType) { robotType_ = robotType; } // set robot type

    string robotName() const { return robotName_; } // get robot name
    void setRobotName(string robotName) { robotName_ = robotName; } // set robot name

    int numOfLives() const { return numOfLives_; } // get number of lives
    void setNumOfLives(int numOfLives) { numOfLives_ = numOfLives; } // set number of lives

    int numOfKills() const { return numOfKills_; } // get number of kills
    void setNumOfKills(int numOfKills) { numOfKills_ = numOfKills; } // set number of kills

    void reduceLives() {
        if (numOfLives_ > 0) {
            numOfLives_--;
        }
    }
    void increaseKills() {
        numOfKills_++;
    }

    bool isAlive() const {
        return numOfLives_ > 0;
    }

    //overloading the << operator for robot class
    friend ostream& operator<<(ostream& out, const Robot& r) {
        out << r.id_ << " at (" << r.robotPosX << ", " << r.robotPosY << ")";
        return out;
    }

    //Pure Virtual Functions
    virtual void setLocations(int x, int y) = 0;
    virtual void actions(Battlefield* battlefield) = 0;
};

class ThinkingRobot: virtual public Robot {
protected:
    //data member
public:
    virtual ~ThinkingRobot() {}

    virtual void actionThink(Battlefield* battlefield) = 0; 
    //virtual function for stepping
};

class SeeingRobot: virtual public Robot {
protected:
    //data member
public:
    virtual ~SeeingRobot() {}

    virtual void actionLook(Battlefield* battlefield) = 0; 
    //virtual function for seeing
};

class ShootingRobot: virtual public Robot {
protected:
    //data member
public:
    virtual ~ShootingRobot() {}

    virtual void actionShoot(Battlefield* battlefield) = 0; 
    //virtual function for shooting
};

class MovingRobot: virtual public Robot {
protected:
    //data member
public:
    virtual ~MovingRobot() {}

    virtual void actionMove(Battlefield* battlefield) = 0; 
    //virtual function for moving
};

class GenericRobot: public ThinkingRobot, public SeeingRobot, 
                    public ShootingRobot, public MovingRobot 
{
private:
    static int robotAutoIncrementInt; // static variable for auto incrementing robot id
    //data member
public:
    GenericRobot(string id = "", int x = -1, int y = -1) {
        id_ = id; // set robot type to GenericRobot
        robotPosX = x; // set x position of robot
        robotPosY = y; // set y position of robot

        robotAutoIncrementInt++; // increment robot id
    }

    static int getRobotAutoIncrementInt() {
        return robotAutoIncrementInt; // return robot id
    }

    virtual ~GenericRobot() {}

    virtual void setLocations(int x, int y) {
        robotPosX = x; // set x position of robot
        robotPosY = y; // set y position of robot
    }

    virtual void actionThink(Battlefield* battlefield) {
        // Implement the logic for thinking robot actions here
        cout << "Thinking Robot " << id_ << " is thinking..." << endl;
    }

    virtual void actionLook(Battlefield* battlefield) {
        // Implement the logic for seeing robot actions here
        cout << "Seeing Robot " << id_ << " is looking..." << endl;
    }

    virtual void actionShoot(Battlefield* battlefield) {
        // Implement the logic for shooting robot actions here
        cout << "Shooting Robot " << id_ << " is shooting..." << endl;
    }

    virtual void actionMove(Battlefield* battlefield) {
        // Implement the logic for moving robot actions here
        cout << "Moving Robot " << id_ << " is moving..." << endl;
    }

    virtual void actions (Battlefield* battlefield) {
        int randomActionThink = 0;

        if (randomActionThink % 2 == 0) {
            actionThink(battlefield);
            actionLook(battlefield);
            actionShoot(battlefield);
            actionMove(battlefield);
        } else if (randomActionThink % 2 == 1) {
            actionLook(battlefield);
            actionShoot(battlefield);
            actionMove(battlefield);
            actionThink(battlefield);
        }
    }
};

int GenericRobot::robotAutoIncrementInt = 0; // initialize static variable

class Battlefield {
private:
    int BATTLEFIELD_NUM_OF_COLS_ = -1;
    int BATTLEFIELD_NUM_OF_ROWS_ = -1;

    int turns_ = 1; // total number of turns
    int turn = 0; // current turn

    int numOfRobots_ = -1; // number of robots

    vector<Robot*> robots_;
    queue<Robot*> destroyedRobots_;
    queue<Robot*> waitingRobots_;
    vector<vector<string>> battlefield_; //2D vector for battlefield

public:
    //Getter Functions
    int BATTLEFIELD_NUM_OF_COLS() const { return BATTLEFIELD_NUM_OF_COLS_; } // get number of columns
    int BATTLEFIELD_NUM_OF_ROWS() const { return BATTLEFIELD_NUM_OF_ROWS_; } // get number of rows
    int turns() const { return turns_; }
    int numOfRobots() const { return numOfRobots_; }
    

    void readFile(string filename) {
        ifstream GameFile(filename);
        string line;

        battlefield_.clear();  

        while (getline(GameFile, line)) {
            vector<string> row;
            row.push_back(line);  

            // check input for map battlefield size
            if (line.find("M by N:") != string::npos) {
                sscanf(line.c_str(), "M by N: %d %d", &BATTLEFIELD_NUM_OF_COLS_, &BATTLEFIELD_NUM_OF_ROWS_);
                cout << "cols = " << BATTLEFIELD_NUM_OF_COLS_ << ", rows = " << BATTLEFIELD_NUM_OF_ROWS_ << endl;

                battlefield_.resize(BATTLEFIELD_NUM_OF_ROWS_, vector<string>(BATTLEFIELD_NUM_OF_COLS_, ""));
            }

            // check input for amount of turns 
            if (line.find("turns:") != string::npos) {
                sscanf(line.c_str(), "turns: %d", &turns_);
                //cout << "turns = " << turns_<<endl;
            }

            // check input for amount of Robots
            if (line.find("robots:") != string::npos) {
                sscanf(line.c_str(), "robots: %d ", &numOfRobots_);
                //cout << "Robots = " << robots_ <<endl;
            }

            // check input for Robots info
            if (line.find("GenericRobot") != string::npos) {
                string tag, robotName, posXStr, posYStr;
                int robotXPos, robotsYPos;

                istringstream iss(line);
                iss >> tag >> robotName >> posXStr >> posYStr;
                
                // when encounter pos input of random 
                int randomX = time(0);
                srand(randomX);

                if (posXStr == "random") {
                    robotXPos = rand() % BATTLEFIELD_NUM_OF_COLS_;  
                } else {
                    robotXPos = stoi(posXStr);
                }

                if (posYStr == "random") {
                    robotsYPos = rand() % BATTLEFIELD_NUM_OF_ROWS_;  
                } else {
                    robotsYPos = stoi(posYStr);
                }

                cout << "Robots in game are = " << robotName << endl;
                cout << "Robot position = (" << robotXPos << ", " << robotsYPos << ")" << endl;

                 // get rid of _ after robotId
                string robotId = robotName;
                robotId = robotId.substr(0,robotName.find('_'));

                GenericRobot* robot = new GenericRobot(robotId, robotXPos, robotsYPos);
                robot->setRobotType("GenericRobot");
                robot->setRobotName(robotName);
                robots_.push_back(robot);
                
            }

        }

        

        GameFile.close();
    }

    void placeRobots() {
        for (int i = 0; i < battlefield_.size(); i++) {
            for (int j = 0; j < battlefield_[i].size(); j++) {
                if (battlefield_[i][j] == " ") {
                    Robot* robot = new GenericRobot(battlefield_[i][j], i, j);
                    robots_.push_back(robot);             // add robot to the vector
                }
            }
        }

        for (int i = 0; i < robots_.size(); i++) {
            if (robots_[i]->y() < battlefield_.size() && robots_[i]->x() < battlefield_[i].size()) {
                battlefield_[robots_[i]->y()][robots_[i]->x()] = robots_[i]->id(); // place robot on the battlefield
            } else {
                cout << "Robot " << robots_[i]->id() << " is out of bounds!" << endl;
                exit(1); // exit the program if robot is out of bounds
            }
        }
    }

    void displayBattlefield() {
        cout << "Display Battlefield" << endl;
        cout << endl << "     ";
        for (int j = 0; j < battlefield_[0].size(); j++) {
            cout << "  " << right << setfill('0') << setw(2) << j << " ";
        }
        cout << endl;

        for (int i = 0; i < battlefield_.size(); i++) {
            cout << "   ";
            for (int j = 0; j < battlefield_[i].size(); j++) {
                cout << "+----";
            }
            cout << "+" << endl;

            cout << " " << right << setfill('0') << setw(2) << i;
            
            // last seem here \/
            for (int j = 0; j < battlefield_[i].size(); j++) {
                if (battlefield_[i][j] == "") {
                    cout << "|" << "    ";
                } else {
                    cout << "|" << left << setfill(' ') << setw(4) << battlefield_[i][j];
                }
            }
            cout << "|" << endl;
        }
        cout << "   ";
        for (int j = 0; j < battlefield_[0].size(); j++) {
            cout << "+----";
        }
        cout << "+" << endl;
    }
};
/*
void GenericRobot::actionThink(Battlefield* battlefield) {
    // Implement the logic for thinking robot actions here
    cout << "GenericRobot actionThink" << endl;
}

void GenericRobot::actionLook(Battlefield* battlefield) {
    // Implement the logic for seeing robot actions here
    cout << "GenericRobot actionLook" << endl;
}

void GenericRobot::actionShoot(Battlefield* battlefield) {
    // Implement the logic for shooting robot actions here
    cout << "GenericRobot actionShoot" << endl;
}

void GenericRobot::actionMove(Battlefield* battlefield) {
    // Implement the logic for moving robot actions here
    cout << "GenericRobot actionMove" << endl;
}
*/

int main() {
    srand(1211109038);

    Battlefield battlefield;

    battlefield.readFile("fileInput1.txt");
    battlefield.placeRobots();
    battlefield.displayBattlefield();

    //Robot* robotGenericRobot = new GenericRobot("GR01", 4, 4);

    //cout << *robotGenericRobot << endl;
    //robotGenericRobot->actions(&battlefield);

    //delete robotGenericRobot;
    //robotGenericRobot = nullptr;

    return 0;
}