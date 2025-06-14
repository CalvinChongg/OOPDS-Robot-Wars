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
// FUNCTIONS : starting base struture, printing battlefield, print only current robot, get set and clear cells,
//             , getter and setter fucntions
// code logic : upgrade class, pointer related, turn-based
// class : scoutbot, trackbot, longshotbot
// extra task :
//
//
// Member_2: TAN YONG XIN
// FUNCTIONS: actionLook
// Code logic: let robot check 3x3 area around itself, show [R] for itself, [ ] for empty, [E] for enemy, log everything to file
// Class: HealthBot, GodBot
// Extra task: UML design
//
//
// Member_3: THAM MEI TING
// FUNCTIONS: actionMove, handle random jump coordinate
// Code logic: jump limit check using jumpCount and MAXjump, turnCounter to control when JukeBot teleport
// Class: JumpBot, JukeBot
// Extra task: UML design
//
//
// Member_4: YAP CHI YI
//FUNCTIONS      : readfile, placeRobots, updateWaitingRobots, returnbattlefield_, waitingrobots, destroyedrobots, returnrobots,
// code logic     : actionShoot 's general shooting logic( not upgrading ) , outputting to a txt file , game ending requirements
// class          : thirtyshotbot, semiautobot, hidebot
// extra task     : change code to non user input, debugging upgrade class
// ********************************************************

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <vector>
#include <queue>
#include <cstdlib>
#include <ctime>
#include <algorithm>

using namespace std;


class Battlefield;

//global vector to store output lines
vector<string> outputLines;




// Base class for all robots
class Robot {
protected:
    int robotPosX = -1;
    int robotPosY = -1;

    string id_ = ""; // id to be displayed on the battlefield
    string robotType_ = "";
    string robotName_ = ""; // id_robotName eg GR05_Star

    int numOfLives_ = 3; // number of lives
    int numOfKills_ = 0; // number of kills

    int upgradeCount = 0; // number of upgrades
    int shell_ = 10; // number of shell

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

    virtual string robotType() const { return robotType_; } // get robot type
    void setRobotType(string robotType) { robotType_ = robotType; } // set robot type

    string robotName() const { return robotName_; } // get robot name
    void setRobotName(string robotName) { robotName_ = robotName; } // set robot name

    int numOfLives() const { return numOfLives_; } // get number of lives
    void setNumOfLives(int numOfLives) { numOfLives_ = numOfLives; } // set number of lives

    int numOfKills() const { return numOfKills_; } // get number of kills
    void setNumOfKills(int numOfKills) { numOfKills_ = numOfKills; } // set number of kills

    int numOfShell() const { return shell_ ;}
    void setNumOfShells(int shell) { shell_ = shell; } // set number of shells

    void decreaseShell(){
        if (shell_ > 0) {
            shell_--;
        }
    }

    virtual bool canBeTargeted() const {
        return true; // By default, all robots can be targeted
    }

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
        outputLines.push_back(r.id_ + " at (" + to_string(r.robotPosX) + ", " + to_string(r.robotPosY) + ")");
        return out;
    }

    //Pure Virtual Functions
    virtual void setLocations(int x, int y) = 0;
    virtual void actions(Battlefield* battlefield) = 0;

    // for limiting upgrade to 2 per robots
    int getUpgradeCount() const {
        return upgradeCount;
    }

    void incrementUpgradeCount() {
        if (upgradeCount < 2)
            upgradeCount++;
    }

    void setUpgradeCount(int count) {
        if (count >= 0 && count <= 2) {
            upgradeCount = count;
        } else {
            cout << "Max UpgradeCount." << endl;
            outputLines.push_back("Max UpgradeCount.");
        }
    }

    bool canUpgrade() const {
        return upgradeCount < 2;
    }
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

    virtual void actionThink(Battlefield* battlefield);

    virtual void actionLook(Battlefield* battlefield);

    virtual void actionShoot(Battlefield* battlefield);

    virtual void actionMove(Battlefield* battlefield);

    virtual void actions (Battlefield* battlefield) {
        cout << "Actions:" << endl;
        cout << "1." << robotType_ << " actionThink" << endl;
        cout << "2." << robotType_ << " actionLook" << endl;
        cout << "3." << robotType_ << " actionMove" << endl;
        cout << "4." << robotType_ << " actionShoot" << endl;



        outputLines.push_back("Actions:");
        outputLines.push_back("1." + robotType_ + " actionThink");
        outputLines.push_back("2." + robotType_ + " actionLook");
        outputLines.push_back("3." + robotType_ + " actionMove" );
        outputLines.push_back("4." + robotType_ + " actionShoot");

        actionThink(battlefield);
        actionLook(battlefield);
        actionMove(battlefield);
        actionShoot(battlefield);

    }
};

int GenericRobot::robotAutoIncrementInt = 0; // initialize static variable
class Battlefield {
private:
    int BATTLEFIELD_NUM_OF_COLS_ = -1;
    int BATTLEFIELD_NUM_OF_ROWS_ = -1;

    int turns_ = 1; // total number of turns
    int turn = 1; // current turn

    int numOfRobots_ = -1; // number of robots

    vector<Robot*> robots_;
    queue<Robot*> destroyedRobots_;
    queue<pair<Robot*, int>> waitingRobots_;
    vector<vector<string>> battlefield_; //2D vector for battlefield


public:
    //Getter Functions
    int BATTLEFIELD_NUM_OF_COLS() const { return BATTLEFIELD_NUM_OF_COLS_; } // get number of columns
    int BATTLEFIELD_NUM_OF_ROWS() const { return BATTLEFIELD_NUM_OF_ROWS_; } // get number of rows
    int turns() const { return turns_; }
    int currentTurn() const { return turn; } // get current turn
    int numOfRobots() const { return numOfRobots_; }
    queue<Robot*>& destroyedRobots() { return destroyedRobots_; } // get destroyed robots
    queue<pair<Robot*, int>>& waitingRobots() { return waitingRobots_; } // get waiting robots
    vector<Robot*>& robots() { return robots_; } // get robots (non-const reference)
    bool isCellEmpty(int x, int y) const { return battlefield_[y][x].empty(); } // Check if the cell is empty
    vector<vector<string>>& returnbattlefield_()  { return battlefield_; }


    void updateWaitingRobots() {
        int size = waitingRobots_.size();
        for (int i = 0; i < size; ++i) {
            auto [robot, turnsWaited] = waitingRobots_.front();
            waitingRobots_.pop();

            if (turnsWaited + 1 >= 5) {
                // Find random empty position
                int newX, newY;
                bool positionFound = false;

                do {
                    newX = rand() % BATTLEFIELD_NUM_OF_COLS_;
                    newY = rand() % BATTLEFIELD_NUM_OF_ROWS_;
                    if (isCellEmpty(newX, newY)) {
                        positionFound = true;
                    }
                } while (positionFound = false);

                // Fallback: linear search if random failed
                if (!positionFound) {
                    for (newY = 0; newY < BATTLEFIELD_NUM_OF_ROWS_; newY++) {
                        for (newX = 0; newX < BATTLEFIELD_NUM_OF_COLS_; newX++) {
                            if (isCellEmpty(newX, newY)) {
                                positionFound = true;
                                goto position_found;
                            }
                        }
                    }
                    position_found:;
                }

                if (positionFound) {
                    // Update robot position
                    robot->setX(newX);
                    robot->setY(newY);
                    setCell(newX, newY, robot);

                    cout << robot->id() << " has waited 5 turns and is returning at (" << newX << "," << newY << ")\n";
                    outputLines.push_back( robot->id() + " has waited 5 turns and is returning at (" + to_string(newX) + "," + to_string(newY) + ")\n");

                    robots_.push_back(robot);
                } else {
                    cout << "No empty space found for " << robot->id() << ", keeping in waiting queue\n";
                    outputLines.push_back( "No empty space found for " + robot->id() + ", keeping in waiting queue\n");
                    waitingRobots_.push({robot, turnsWaited + 1});
                }
            } else {
                waitingRobots_.push({robot, turnsWaited + 1});
            }
        }
    }

    string getCellContent(int x, int y) const {
        if (y >= 0 && y < BATTLEFIELD_NUM_OF_ROWS_ && x >= 0 && x < BATTLEFIELD_NUM_OF_COLS_) {
            return battlefield_[y][x];
            cout << "Cell content at (" << x << ", " << y << ") is: " << battlefield_[y][x] << "\n"; /*troubleshooting*/
            outputLines.push_back( "Cell content at (" + to_string(x) + ", " + to_string(y) + ") is: " + battlefield_[y][x]);
        } else {
            return "";
        }
    }

    // Setter Functions
    void setCurrentTurn(int turn) { this->turn = turn; } // set current turn

    void setCell(int x, int y, Robot* robot) {
        if (robot == nullptr) {
            battlefield_[y][x] = "";
        } else {
            battlefield_[y][x] = robot->id(); // or however you show the robot on screen
        }
    }

    void clearCell(int x, int y) {
        if (y < battlefield_.size() && x < battlefield_[y].size()) {
            battlefield_[y][x] = "";
        }
    }

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
                outputLines.push_back("cols = " + to_string(BATTLEFIELD_NUM_OF_COLS_) + ", rows = " + to_string(BATTLEFIELD_NUM_OF_ROWS_));

                battlefield_.resize(BATTLEFIELD_NUM_OF_ROWS_, vector<string>(BATTLEFIELD_NUM_OF_COLS_, ""));
            }

            // check input for amount of turns
            if (line.find("turns:") != string::npos) {
                sscanf(line.c_str(), "turns: %d", &turns_);
            }

            // check input for amount of Robots
            if (line.find("robots:") != string::npos) {
                sscanf(line.c_str(), "robots: %d ", &numOfRobots_);
            }

            // check input for Robots info
            if (line.find("GenericRobot") != string::npos) {
                string tag, robotName, posXStr, posYStr;
                int robotXPos, robotsYPos;

                istringstream iss(line);
                iss >> tag >> robotName >> posXStr >> posYStr;

                // when encounter pos input of random

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
                outputLines.push_back("Robots in game are = " + robotName);
                outputLines.push_back("Robot position = (" + to_string(robotXPos)  + ", " + to_string(robotsYPos)  + ")");

                 // get rid of _ after robotId
                string robotId = robotName;
                robotId = robotId.substr(0,robotName.find('_'));

                GenericRobot* robot = new GenericRobot(robotId, robotXPos, robotsYPos);
                robot->setRobotType(tag);
                robot->setRobotName(robotName);
                robots_.push_back(robot);

            }

        }



        GameFile.close();
    }

    void placeCurrentRobot(Robot* robot, int currentTurn, int robotCount) {
        // Clear battlefield display grid first
        for (int i = 0; i < battlefield_.size(); ++i) {
            for (int j = 0; j < battlefield_[i].size(); ++j) {
                battlefield_[i][j] = "";
            }
        }

        int robotIndex = (currentTurn % robotCount);
        Robot* currentRobot = robots_[robotIndex];

        if (robot != currentRobot) return;

        int x = robot->x();
        int y = robot->y();

        if (y < battlefield_.size() && x < battlefield_[y].size()) {
            setCell(x, y, robot);
        } else {
            cout << "Robot " << robot->id() << " is out of bounds!" << endl;
            outputLines.push_back("Robot " + robot->id() + " is out of bounds!");
            exit(1);
        }
    }



    void placeRobots() {
        for (auto robot : robots_) {
            string robotID = robot->id();

            // Check if robot is in destroyedRobots_
            bool isDestroyed = false;
            queue<Robot*> tempDestroyed = destroyedRobots_;
            while (!tempDestroyed.empty()) {
                Robot* r = tempDestroyed.front();
                tempDestroyed.pop();
                if (r->id() == robotID) {
                    isDestroyed = true;
                    break;
                }
            }

            // Check if robot is in waitingRobots_
            bool isWaiting = false;
            queue<pair<Robot*, int>> tempWaiting = waitingRobots_;
            while (!tempWaiting.empty()) {
                Robot* r = tempWaiting.front().first;
                tempWaiting.pop();
                if (r->id() == robotID) {
                    isWaiting = true;
                    break;
                }
            }

            // Place robot only if it's active
            if (!isDestroyed && !isWaiting) {
                int x = robot->x();
                int y = robot->y();

                if (y < battlefield_.size() && x < battlefield_[y].size()) {
                    setCell(x, y, robot);
                } else {
                    cout << "Robot " << robot->id() << " is out of bounds!" << endl;
                    exit(1);
                }
            }
        }
    }

    void displayBattlefield() {
        cout << "Display Battlefield " << endl;
        outputLines.push_back("Display Battlefield ");

        cout << endl << "     ";
        string header = "     ";
        for (int j = 0; j < battlefield_[0].size(); j++) {
            cout << "  " << right << setfill('0') << setw(2) << j << " ";
            ostringstream oss;
            oss << "  " << right << setfill('0') << setw(2) << j << " ";
            header += oss.str();
        }
        cout << endl;
        outputLines.push_back(header);

        for (int i = 0; i < battlefield_.size(); i++) {
            // Draw top border line
            cout << "   ";
            string topLine = "   ";
            for (int j = 0; j < battlefield_[i].size(); j++) {
                cout << "+----";
                topLine += "+----";
            }
            cout << "+" << endl;
            topLine += "+";
            outputLines.push_back(topLine);

            // Draw row content
            cout << " " << right << setfill('0') << setw(2) << i;
            ostringstream rowLine;
            rowLine << " " << right << setfill('0') << setw(2) << i;

            for (int j = 0; j < battlefield_[i].size(); j++) {

                if (battlefield_[i][j] == "") {
                    cout << "|" << "    ";
                    rowLine << "|    ";
                } else {
                    cout << "|" << left << setfill(' ') << setw(4) << battlefield_[i][j];
                    rowLine << "|" << left << setw(4) << battlefield_[i][j];
                }


            }
            cout << "|" << endl;
            rowLine << "|";
            outputLines.push_back(rowLine.str());
        }

        // Draw bottom border
        cout << "   ";
        string bottomLine = "   ";
        for (int j = 0; j < battlefield_[0].size(); j++) {
            cout << "+----";
            bottomLine += "+----";
        }
        cout << "+" << endl;
        bottomLine += "+";
        outputLines.push_back(bottomLine);
    }
};



// Seeing Upgrades
class ScoutBot: public ThinkingRobot, public SeeingRobot,
                public ShootingRobot, public MovingRobot
{
private:
    int LookAllCount = 3;
public:
    ScoutBot(string id = "", int x = -1, int y = -1) {
        id_ = "SB" + id.substr(2,2);
        robotType_ = "ScoutBot";
        robotPosX = x;
        robotPosY = y;
        robotName_ ;


    }

    virtual ~ScoutBot() {}

    void setLocations(int x, int y) override {
        robotPosX = x;
        robotPosY = y;
    }

    void actionThink(Battlefield* battlefield) override;
    void actionLook(Battlefield* battlefield) override;
    void actionMove(Battlefield* battlefield) override;
    void actionShoot(Battlefield* battlefield) override;
    void Showallbot(Battlefield* battlefield) ;


    void actions(Battlefield* battlefield) override {

        cout << "Actions:" << endl;
        cout << "1. ScoutBot actionLook" << endl;
        cout << "2. ScoutBot actionThink" << endl;
        cout << "3. ScoutBot actionMove" << endl;
        cout << "4. ScoutBot actionShoot" << endl;

        outputLines.push_back("Actions:");
        outputLines.push_back("1. ScoutBot actionThink" );
        outputLines.push_back("2. ScoutBot actionLook" );
        outputLines.push_back("3. ScoutBot actionMove" );
        outputLines.push_back("4. ScoutBot actionShoot" );

        actionThink(battlefield);
        actionLook(battlefield);
        actionMove(battlefield);
        actionShoot(battlefield);

    }
};

class TrackBot: public ThinkingRobot, public SeeingRobot,
                public ShootingRobot, public MovingRobot
{
private:
    vector<Robot*> trackedRobots_;
    static const int MAX_TRACKERS = 3;
public:
    TrackBot(string id = "", int x = -1, int y = -1) {
        id_ = "TB" + id.substr(2,2);
        robotType_ = "TrackBot";
        robotPosX = x;
        robotPosY = y;
        robotName_ ;
    }

    virtual ~TrackBot() {}

    void setLocations(int x, int y) override {
        robotPosX = x;
        robotPosY = y;
    }

    void actionThink(Battlefield* battlefield) override;
    void actionLook(Battlefield* battlefield) override;
    void actionMove(Battlefield* battlefield) override;
    void actionShoot(Battlefield* battlefield) override;
    void plantTracker(Battlefield* battlefield);

    void actions(Battlefield* battlefield) override {

        cout << "Actions:" << endl;
        cout << "1. TrackBot actionThink" << endl;
        cout << "2. TrackBot actionLook" << endl;
        cout << "3. TrackBot actionMove" << endl;
        cout << "4. TrackBot actionShoot" << endl;
        cout << "5. TrackBot PlantTracker" << endl;

        outputLines.push_back("Actions:");
        outputLines.push_back("1. TrackBot actionThink" );
        outputLines.push_back("2. TrackBot actionLook" );
        outputLines.push_back("3. TrackBot actionMove" );
        outputLines.push_back("4. TrackBot actionShoot" );
        outputLines.push_back("5. TrackBot PlantTracker");


        actionThink(battlefield);
        actionLook(battlefield);
        actionShoot(battlefield);
        actionMove(battlefield);

    }
};



// Shooting Upgrades
class LongShotBot: public ThinkingRobot, public SeeingRobot,
                public ShootingRobot, public MovingRobot
{
public:
    LongShotBot(string id = "", int x = -1, int y = -1) {
        id_ = "LS" + id.substr(2,2);
        robotType_ = "LongShotBot";
        robotPosX = x;
        robotPosY = y;
        robotName_ ;
    }

    virtual ~LongShotBot() {}

    void setLocations(int x, int y) override {
        robotPosX = x;
        robotPosY = y;
    }

    void actionThink(Battlefield* battlefield) override;
    void actionLook(Battlefield* battlefield) override;
    void actionMove(Battlefield* battlefield) override;
    void actionShoot(Battlefield* battlefield) override;

    void actions(Battlefield* battlefield) override {

        cout << "Actions:" << endl;
        cout << "1. LongShotBot actionThink" << endl;
        cout << "2. LongShotBot actionLook" << endl;
        cout << "3. LongShotBot actionMove" << endl;
        cout << "4. LongShotBot actionShoot" << endl;

        outputLines.push_back("Actions:");
        outputLines.push_back("1. LongShotBot actionThink" );
        outputLines.push_back("2. LongShotBot actionLook" );
        outputLines.push_back("3. LongShotBot actionMove" );
        outputLines.push_back("4. LongShotBot actionShoot" );

        actionThink(battlefield);
        actionLook(battlefield);
        actionShoot(battlefield);
        actionMove(battlefield);

    }
};

class SemiAutoBot: public ThinkingRobot, public SeeingRobot,
                public ShootingRobot, public MovingRobot
{
public:
    SemiAutoBot(string id = "", int x = -1, int y = -1) {
        id_ = "SA" + id.substr(2,2);
        robotType_ = "SemiAutoBot";
        robotPosX = x;
        robotPosY = y;
        robotName_ ;
    }

    virtual ~SemiAutoBot() {}

    void setLocations(int x, int y) override {
        robotPosX = x;
        robotPosY = y;
    }

    void actionThink(Battlefield* battlefield) override;
    void actionLook(Battlefield* battlefield) override;
    void actionMove(Battlefield* battlefield) override;
    void actionShoot(Battlefield* battlefield) override;

    void actions(Battlefield* battlefield) override {
        cout << "Actions:" << endl;
        cout << "1. SemiAutoBot actionThink" << endl;
        cout << "2. SemiAutoBot actionLook" << endl;
        cout << "3. SemiAutoBot actionMove" << endl;
        cout << "4. SemiAutoBot actionShoot" << endl;

        outputLines.push_back("Actions:");
        outputLines.push_back("1. SemiAutoBot actionThink" );
        outputLines.push_back("2. SemiAutoBot actionLook" );
        outputLines.push_back("3. SemiAutoBot actionMove" );
        outputLines.push_back("4. SemiAutoBot actionShoot" );

        actionThink(battlefield);
        actionLook(battlefield);
        actionShoot(battlefield);
        actionMove(battlefield);

    }
};

class ThirtyShotBot: public ThinkingRobot, public SeeingRobot,
                public ShootingRobot, public MovingRobot
{
public:
    ThirtyShotBot(string id = "", int x = -1, int y = -1) {
        id_ = "TS" + id.substr(2,2);
        robotType_ = "ThirtyShotBot";
        robotPosX = x;
        robotPosY = y;
        robotName_ ;
        setNumOfShells(30);
    }

    virtual ~ThirtyShotBot() {}

    void setLocations(int x, int y) override {
        robotPosX = x;
        robotPosY = y;
    }

    void actionThink(Battlefield* battlefield) override;
    void actionLook(Battlefield* battlefield) override;
    void actionMove(Battlefield* battlefield) override;
    void actionShoot(Battlefield* battlefield) override;

    void actions(Battlefield* battlefield) override {
        cout << "Actions:" << endl;
        cout << "1. ThirtyShotBot actionThink" << endl;
        cout << "2. ThirtyShotBot actionLook" << endl;
        cout << "3. ThirtyShotBot actionMove" << endl;
        cout << "4. ThirtyShotBot actionShoot" << endl;

        outputLines.push_back("Actions:");
        outputLines.push_back("1. ThirtyShotBot actionThink" );
        outputLines.push_back("2. ThirtyShotBot actionLook" );
        outputLines.push_back("3. ThirtyShotBot actionMove" );
        outputLines.push_back("4. ThirtyShotBot actionShoot" );

        actionThink(battlefield);
        actionLook(battlefield);
        actionShoot(battlefield);
        actionMove(battlefield);

    }
};



// Moving Upgrades
class HideBot: public ThinkingRobot, public SeeingRobot,
                public ShootingRobot, public MovingRobot
{
protected:
    int hiddenCount = 3;
    bool isHidden = false;
public:
    HideBot(string id = "", int x = -1, int y = -1) {
        id_ = "HB" + id.substr(2,2);
        robotType_ = "HideBot";
        robotPosX = x;
        robotPosY = y;
        robotName_ ;
    }

    void setLocations(int x, int y) override {
        robotPosX = x;
        robotPosY = y;
    }

    bool canBeTargeted() const override {
        return !isHidden; // HideBot can only be targeted when not hidden
    }

    void actionThink(Battlefield* battlefield) override;
    void actionLook(Battlefield* battlefield) override;
    void actionMove(Battlefield* battlefield) override;
    void actionShoot(Battlefield* battlefield) override;

    void actions(Battlefield* battlefield) override {

        cout << "Actions:" << endl;
        cout << "1. HideBot actionThink" << endl;
        cout << "2. HideBot actionLook" << endl;
        cout << "3. HideBot actionMove" << endl;
        cout << "4. HideBot actionShoot" << endl;
        cout << "5. HideBot Hide" <<endl;

        outputLines.push_back("Actions:");
        outputLines.push_back("1. HideBot actionThink" );
        outputLines.push_back("2. HideBot actionLook" );
        outputLines.push_back("3. HideBot actionMove" );
        outputLines.push_back("4. HideBot actionShoot" );
        outputLines.push_back("5. HideBot Hide");


        actionThink(battlefield);
        actionLook(battlefield);
        actionShoot(battlefield);
        actionMove(battlefield);

    }
};

class JumpBot: public ThinkingRobot, public SeeingRobot,
                public ShootingRobot, public MovingRobot
{
private:
    int jumpCount = 0;
    static const int MAX_JUMPS = 3;

public:
    JumpBot(string id = "", int x = -1, int y = -1) {
        id_ = "JB" + id.substr(2,2);
        robotType_ = "JumpBot";
        robotPosX = x;
        robotPosY = y;
        robotName_ ;
    }

    void setLocations(int x, int y) override {
        robotPosX = x;
        robotPosY = y;
    }

    void actionThink(Battlefield* battlefield) override;
    void actionLook(Battlefield* battlefield) override;
    void actionMove(Battlefield* battlefield) override;
    void actionShoot(Battlefield* battlefield) override;

    void actions(Battlefield* battlefield) override {
        cout << "Actions:" << endl;
        cout << "1. JumpBot actionThink" << endl;
        cout << "2. JumpBot actionLook" << endl;
        cout << "3. JumpBot actionMove" << endl;
        cout << "4. JumpBot actionShoot" << endl;

        outputLines.push_back("Actions:");
        outputLines.push_back("1. JumpBot actionThink" );
        outputLines.push_back("2. JumpBot actionLook" );
        outputLines.push_back("3. JumpBot actionMove" );
        outputLines.push_back("4. JumpBot actionShoot" );

        actionThink(battlefield);
        actionLook(battlefield);
        actionShoot(battlefield);
        actionMove(battlefield);

    }

};



// Extras Upgrades
class HealthBot: public ThinkingRobot, public SeeingRobot,
                public ShootingRobot, public MovingRobot
{
public:
    HealthBot(string id = "", int x = -1, int y = -1) {
        id_ = "HB" + id.substr(2,2);
        robotType_ = "HealthBot";
        robotPosX = x;
        robotPosY = y;
        robotName_ ;
        setNumOfLives(5);
    }

    void setLocations(int x, int y) override {
        robotPosX = x;
        robotPosY = y;
    }

    void actionThink(Battlefield* battlefield) override;
    void actionLook(Battlefield* battlefield) override;
    void actionMove(Battlefield* battlefield) override;
    void actionShoot(Battlefield* battlefield) override;

    void actions(Battlefield* battlefield) override {
        cout << "Actions:" << endl;
        cout << "1. HealthBot actionThink" << endl;
        cout << "2. HealthBot actionLook" << endl;
        cout << "3. HealthBot actionMove" << endl;
        cout << "4. HealthBot actionShoot" << endl;

        outputLines.push_back("Actions:");
        outputLines.push_back("1. HealthBot actionThink" );
        outputLines.push_back("2. HealthBot actionLook" );
        outputLines.push_back("3. HealthBot actionMove" );
        outputLines.push_back("4. HealthBot actionShoot" );

        actionThink(battlefield);
        actionLook(battlefield);
        actionShoot(battlefield);
        actionMove(battlefield);

    }
};

class JukeBot: public ThinkingRobot, public SeeingRobot,
                public ShootingRobot, public MovingRobot
{
private:
    int turnCounter = 0;

public:
    JukeBot(string id = "", int x = -1, int y = -1) {
        id_ = "JK" + id.substr(2,2);
        robotType_ = "JukeBot";
        robotPosX = x;
        robotPosY = y;
        robotName_ ;
    }

    void setLocations(int x, int y) override {
        robotPosX = x;
        robotPosY = y;
    }

    void actionThink(Battlefield* battlefield) override;
    void actionLook(Battlefield* battlefield) override;
    void actionMove(Battlefield* battlefield) override;
    void actionShoot(Battlefield* battlefield) override;

    void actions(Battlefield* battlefield) override {
        turnCounter++;
        if (turnCounter % 5 == 0) {
            // Juke to random position
            int newX, newY;
            bool positionFound = false;

            // Try random positions first
            for (int attempt = 0; attempt < 100; attempt++) {
                newX = rand() % battlefield->BATTLEFIELD_NUM_OF_COLS();
                newY = rand() % battlefield->BATTLEFIELD_NUM_OF_ROWS();

                if (battlefield->isCellEmpty(newX, newY)) {
                    positionFound = true;
                    break;
                }
            }

            if (positionFound) {
                battlefield->setCell(robotPosX, robotPosY, nullptr);
                robotPosX = newX;
                robotPosY = newY;
                battlefield->setCell(robotPosX, robotPosY, this);
                cout << id_ << " juked to (" << newX << "," << newY << ")!" << endl;
                outputLines.push_back( id_ + " juked to (" + to_string(newX)  + "," + to_string(newY) + ")!");

            } else {
                cout << "No empty space found for juke!" << endl;
                outputLines.push_back("No empty space found for juke!");

            }
        }


        cout << "Actions:" << endl;
        cout << "1. JukeBot actionMove" << endl;
        cout << "2. JukeBot actionShoot" << endl;
        cout << "3. JukeBot actionLook" << endl;
        cout << "4. JukeBot actionThink" << endl;

        outputLines.push_back("Actions:");
        outputLines.push_back("1. JukeBot actionMove" );
        outputLines.push_back("2. JukeBot actionShoot" );
        outputLines.push_back("3. JukeBot actionLook" );
        outputLines.push_back("4. JukeBot actionThink" );

        actionThink(battlefield);
        actionLook(battlefield);
        actionShoot(battlefield);
        actionMove(battlefield);



    }
};

class GodBot: public ThinkingRobot, public SeeingRobot,
                public ShootingRobot, public MovingRobot
{
public:
    GodBot(string id = "", int x = -1, int y = -1) {
        id_ = "GB" + id.substr(2,2);
        robotType_ = "GodBot";
        robotPosX = x;
        robotPosY = y;
        robotName_ ;
    }

    void setLocations(int x, int y) override {
        robotPosX = x;
        robotPosY = y;
    }

    void actionThink(Battlefield* battlefield) override;
    void actionLook(Battlefield* battlefield) override;
    void actionMove(Battlefield* battlefield) override;
    void actionShoot(Battlefield* battlefield) override;

    bool canBeTargeted() const override {
        return false; // GodBot cannot be targeted
    }

    void actions(Battlefield* battlefield) override {

        cout << "Actions:" << endl;
        cout << "1. GodBot actionThink" << endl;
        cout << "2. GodBot actionLook" << endl;
        cout << "3. GodBot actionMove" << endl;
        cout << "4. GodBot actionShoot" << endl;


        outputLines.push_back("Actions:");
        outputLines.push_back("1. GodBot actionThink" );
        outputLines.push_back("2. GodBot actionLook" );
        outputLines.push_back("3. GodBot actionMove" );
        outputLines.push_back("4. GodBot actionShoot" );

        actionThink(battlefield);
        actionLook(battlefield);
        actionShoot(battlefield);
        actionMove(battlefield);


    }
};


//GenericBot Action Logics
void GenericRobot::actionThink(Battlefield* battlefield) {
    cout << "GenericRobot is Thinking..." << endl;
    outputLines.push_back("GenericRobot is Thinking...");
}

void GenericRobot::actionLook(Battlefield* battlefield) {
    int currentX = robotPosX;
    int currentY = robotPosY;

    cout << "=== Robot View (3x3 Grid) ===" << endl;
    outputLines.push_back("=== Robot View (3x3 Grid) ===");

    for (int dy = -1; dy <= 1; ++dy) {
        string row = "";
        for (int dx = -1; dx <= 1; ++dx) {
            int x = currentX + dx;
            int y = currentY + dy;

            if (y >= 0 && y < battlefield->BATTLEFIELD_NUM_OF_ROWS() &&
                x >= 0 && x < battlefield->BATTLEFIELD_NUM_OF_COLS()) {

                if (x == currentX && y == currentY) {
                    cout << "[R] ";
                    row += "[R] ";
                } else {
                    string content = battlefield->getCellContent(x, y);
                    if (content.empty()) {
                        cout << "[ ] ";
                        row += "[ ] ";
                    } else {
                        cout << "[" << content.substr(0, 1) << "] ";
                        row += "[" + content.substr(0, 1) + "] ";
                    }
                }
            } else {
                cout << "[#] ";
                row += "[#] ";
            }
        }
        cout << endl;
        outputLines.push_back(row);
    }
}

void GenericRobot::actionMove(Battlefield* battlefield) {
    int currentX = robotPosX;
    int currentY = robotPosY;

    vector<string> directions = {"UL", "U", "UR", "L", "C", "R", "DL", "D", "DR"};
    vector<pair<int, int>> directionsMove = {
        {-1, -1}, {0, -1}, {1, -1},
        {-1, 0},  {0, 0},  {1, 0},
        {-1, 1},  {0, 1},  {1, 1}
    };

    cout << "\nAvailable Directions:\n";
    cout << "UL, U, UR\n";
    cout << "L, C, R\n";
    cout << "DL, D, DR\n";

    outputLines.push_back("\nAvailable Directions:");
    outputLines.push_back("UL, U, UR");
    outputLines.push_back("L, C, R");
    outputLines.push_back("DL, D, DR");

    // store valid index of move location  into validIndex
    vector<int> validIndex;
    for (int i = 0; i < directions.size(); ++i) {
        int newX = currentX + directionsMove[i].first;
        int newY = currentY + directionsMove[i].second;

        // Check bounds
        if (newX < 0 || newX >= battlefield->BATTLEFIELD_NUM_OF_COLS() ||
            newY < 0 || newY >= battlefield->BATTLEFIELD_NUM_OF_ROWS())
            continue;

        // Check if destination is occupied
        bool isOccupied = false;
        for (Robot* robot : battlefield->robots()) {
            if (robot->x() == newX && robot->y() == newY) {
                isOccupied = true;
                break;
            }
        }

        if (!isOccupied) {
            validIndex.push_back(i); // push back the index of the valid move
        }
    }

    // Handle case when no valid directions
    if (validIndex.empty()) {
        cout << "No valid move directions available\n";
        outputLines.push_back("No valid move directions available");
        return;
    }

    // Randomly pick a valid direction
    int chosenIndex = validIndex[rand() % validIndex.size()];
    string userInput = directions[chosenIndex];

    cout << "Random direction selected: " << userInput << endl;
    outputLines.push_back("Random direction selected: " + userInput);

    // Calculate new position
    int newX = currentX + directionsMove[chosenIndex].first;
    int newY = currentY + directionsMove[chosenIndex].second;

    // Update position
    robotPosX = newX;
    robotPosY = newY;
    battlefield->setCell(currentX, currentY, nullptr);
    battlefield->setCell(currentX, currentY, nullptr);
    battlefield->setCell(newX, newY, this);

    cout << "Moved to (" << newX << "," << newY << ")" << endl;
    outputLines.push_back("Moved to (" + to_string(newX) + "," + to_string(newY) + ")");

    // debug purposes
    // for (int i = 0; i < validIndex.size(); ++i) {
    //     cout << "Valid direction index: " << validIndex[i] << " - " << directions[validIndex[i]] << endl;
    //     outputLines.push_back("Valid direction index: " + to_string(validIndex[i]) + " - " + directions[validIndex[i]]);
    // }
}

void GenericRobot::actionShoot(Battlefield* battlefield) {

    string CurrentRobotsName = this->robotName();
    cout << "The Current Robot controlled is " << CurrentRobotsName << endl;
    outputLines.push_back("The Current Robot controlled is " + CurrentRobotsName);

    int CurrentRobotsX = this->x();
    int CurrentRobotsY = this->y();

    int targetX, targetY;
    bool validTarget = false;

    int battlefieldWidth = battlefield->BATTLEFIELD_NUM_OF_COLS();
    int battlefieldHeight = battlefield->BATTLEFIELD_NUM_OF_ROWS();

    // Generate list of valid adjacent targets
    vector<pair<int, int>> possibleTargets;
    for (int dx = -1; dx <= 1; dx++) {
        for (int dy = -1; dy <= 1; dy++) {
            if (dx == 0 && dy == 0) continue; // Skip self

            int newX = CurrentRobotsX + dx;
            int newY = CurrentRobotsY + dy;

            // Check if within bounds
            if (newX >= 0 && newX < battlefieldWidth && newY >= 0 && newY < battlefieldHeight) {
                possibleTargets.emplace_back(newX, newY);
            }
        }
    }

    if (!possibleTargets.empty()) {
        int index = rand() % possibleTargets.size();
        targetX = possibleTargets[index].first;
        targetY = possibleTargets[index].second;

        cout << "Random target selected at: " << targetX << " " << targetY << endl;
        outputLines.push_back("Random target selected at: " + to_string(targetX) + " " + to_string(targetY));

        validTarget = true;
    } else {
        cout << "No valid adjacent tiles found for targeting." << endl;
        outputLines.push_back("No valid adjacent tiles found for targeting.");
        return;
    }

    // Shell check
    int ShellLeft = this->numOfShell();
    if (ShellLeft == 0) {
        cout << "Oh no! You ran out of shells! You can't shoot anything! YOU ARE SELF DESTRUCTING!" << endl;
        outputLines.push_back("Oh no! You ran out of shells! You can't shoot anything! YOU ARE SELF DESTRUCTING!");
        this->reduceLives();
        this->setNumOfShells(10);

        return;
    }

    bool hit = false;

    for (Robot* robot : battlefield->robots()) {
        string targetRobotId = robot->id() ;
        int PotentialRobotX = robot->x() ;
        int PotentialRobotY = robot->y() ;
        if (targetX == CurrentRobotsX && targetY == CurrentRobotsY ){
            cout<<"\nYou can't Shoot Yourself"<<endl;
            outputLines.push_back("\nYou can't Shoot Yourself");

            break;
        }
        if (targetX == PotentialRobotX && targetY == PotentialRobotY ){
            if (!robot->canBeTargeted()) {
                cout << "Target is hidden and cannot be hit!" << endl;
                outputLines.push_back("Target is hidden and cannot be hit!");
                return;
            }
            int hitChance = rand() % 100; // number from 0-99
            if (hitChance < 70) { // 70% chance to hit
                cout<<"\nYou've successfully shot an enemy Robot!"<<endl;
                outputLines.push_back("\nYou've successfully shot an enemy Robot!");

                robot->reduceLives();

                if (!robot->isAlive()) {
                    cout << targetRobotId << " has been destroyed!" << endl;
                    outputLines.push_back(targetRobotId + " has been destroyed!");


                    battlefield->setCell(PotentialRobotX, PotentialRobotY, nullptr); // remove robot from battlefield
                    battlefield->clearCell(PotentialRobotX, PotentialRobotY);
                    robot->setX(-1);
                    robot->setY(-2);

                    battlefield->destroyedRobots().push(robot);  // add to destroyed queue

                    // // Optional: If destroyed robots should be removed from active list
                    // auto& robotsVec = battlefield->robots();
                    // auto it = find(robotsVec.begin(), robotsVec.end(), robot);
                    // if (it != robotsVec.end()) {
                    //     robotsVec.erase(it);
                    // }

                } else {
                    cout << targetRobotId << " was hit and is now temporarily inactive!" << endl;
                    outputLines.push_back(targetRobotId + " was hit and is now temporarily inactive!");

                    battlefield->setCell(PotentialRobotX, PotentialRobotY, nullptr); // remove temporarily
                    battlefield->clearCell(PotentialRobotX, PotentialRobotY);
                    robot->setX(-1);
                    robot->setY(-1);

                    battlefield->waitingRobots().push({robot, 0}); // add to waiting queue

                    // auto& robotsVec = battlefield->robots();
                    // auto it = find(robotsVec.begin(), robotsVec.end(), robot);
                    // if (it != robotsVec.end()) {
                    //     robotsVec.erase(it);
                    // }
                }


                this->increaseKills();
                this->decreaseShell();
                int lifeLeft = robot->numOfLives();

                cout<< targetRobotId<<" now has "<<lifeLeft<<" of lives left"<<endl;
                cout<< this->id() <<" now has "<< this->numOfKills() <<" of kills!"<<endl;
                cout<< this->id() <<" now has "<< this->numOfShell() <<" of shells left!"<<endl;

                outputLines.push_back(targetRobotId + " now has " + to_string(lifeLeft) + " of lives left");
                outputLines.push_back(this->id() + " now has " + to_string(this->numOfKills()) + " of kills!");
                outputLines.push_back(this->id() + " now has " + to_string(this->numOfShell()) + " of shells left!");

                // cout << "Remaining Robots: " << endl;
                // for (const auto& r : battlefield->robots()) {
                //     cout << *r << endl;
                // }


                if (this->canUpgrade()) {
                    this->incrementUpgradeCount();

                    cout<<"\n" << this->id() <<" earned an upgrade! A random upgrade is applied!" << endl;
                    outputLines.push_back("\n" + this->id() + " earned an upgrade! A random upgrade is applied!");

                    //Randomly choose upgrade category (1 - Moving, 2 - Shooting, 3 - Extras)
                    int choice = rand() % 3 + 1;

                    switch (choice) {
                        case 1: {
                            // Randomly choose Moving upgrade (1 - HideBot, 2 - JumpBot)
                            int moveChoice = rand() % 2 + 1;
                            if (moveChoice == 1) {
                                cout <<this->id() <<" are now upgraded to HideBot!"<< endl;
                                outputLines.push_back(this->id() + " are now upgraded to HideBot!");

                                HideBot* upgradedRobot = new HideBot(this->id(), this->x(), this->y());

                                // Copy state
                                upgradedRobot->setNumOfLives(this->numOfLives());
                                upgradedRobot->setNumOfKills(this->numOfKills());
                                upgradedRobot->setNumOfShells(this->numOfShell());
                                upgradedRobot->setUpgradeCount(this->getUpgradeCount());

                                int robotName_pos = this->robotName().find("_");
                                upgradedRobot->setRobotName(upgradedRobot->id() + this->robotName().substr(robotName_pos));

                                // Replace the robot in the battlefield vector immediately
                                for (auto& robot : battlefield->robots()) {
                                    if (robot == this) {
                                        robot = upgradedRobot;
                                        break; // important: break to avoid double replacement
                                    }
                                }

                                return;
                            } else {
                                cout <<this->id() <<" are now upgraded to JumpBot!"<< endl;
                                outputLines.push_back(this->id() + " are now upgraded to JumpBot!");
                                JumpBot* upgradedRobot = new JumpBot(this->id(), this->x(), this->y());

                                // Copy state
                                upgradedRobot->setNumOfLives(this->numOfLives());
                                upgradedRobot->setNumOfKills(this->numOfKills());
                                upgradedRobot->setNumOfShells(this->numOfShell());
                                upgradedRobot->setUpgradeCount(this->getUpgradeCount());

                                int robotName_pos = this->robotName().find("_");
                                upgradedRobot->setRobotName(upgradedRobot->id() + this->robotName().substr(robotName_pos));

                                // Replace the robot in the battlefield vector immediately
                                for (auto& robot : battlefield->robots()) {
                                    if (robot == this) {
                                        robot = upgradedRobot;
                                        break; // important: break to avoid double replacement
                                    }
                                }

                                return;
                            }
                        }
                        case 2: {
                            // Randomly choose Shooting upgrade (1 - LongShotBot, 2 - SemiAutoBot, 3 - ThirtyShotBot)
                            int shootChoice = rand() % 4 + 1;
                            if (shootChoice == 1) {
                                cout <<this->id() <<" are now upgraded to LongShotBot!"<< endl;
                                outputLines.push_back(this->id() + " are now upgraded to LongShotBot!");
                                LongShotBot* upgradedRobot = new LongShotBot(this->id(), this->x(), this->y());

                                // Copy state
                                upgradedRobot->setNumOfLives(this->numOfLives());
                                upgradedRobot->setNumOfKills(this->numOfKills());
                                upgradedRobot->setNumOfShells(this->numOfShell());
                                upgradedRobot->setUpgradeCount(this->getUpgradeCount());

                                int robotName_pos = this->robotName().find("_");
                                upgradedRobot->setRobotName(upgradedRobot->id() + this->robotName().substr(robotName_pos));

                                // Replace the robot in the battlefield vector immediately
                                for (auto& robot : battlefield->robots()) {
                                    if (robot == this) {
                                        robot = upgradedRobot;
                                        break; // important: break to avoid double replacement
                                    }
                                }

                                return;
                            } else if (shootChoice == 2) {
                                cout <<this->id() <<" are now upgraded to SemiAutoBot!"<< endl;
                                outputLines.push_back(this->id() + " are now upgraded to SemiAutoBot!");
                                SemiAutoBot* upgradedRobot = new SemiAutoBot(this->id(), this->x(), this->y());

                                // Copy state
                                upgradedRobot->setNumOfLives(this->numOfLives());
                                upgradedRobot->setNumOfKills(this->numOfKills());
                                upgradedRobot->setNumOfShells(this->numOfShell());
                                upgradedRobot->setUpgradeCount(this->getUpgradeCount());

                                int robotName_pos = this->robotName().find("_");
                                upgradedRobot->setRobotName(upgradedRobot->id() + this->robotName().substr(robotName_pos));

                                // Replace the robot in the battlefield vector immediately
                                for (auto& robot : battlefield->robots()) {
                                    if (robot == this) {
                                        robot = upgradedRobot;
                                        break; // important: break to avoid double replacement
                                    }
                                }

                                return;
                            } else {
                                cout <<this->id() <<" are now upgraded to ThirtyShotBot!"<< endl;
                                outputLines.push_back(this->id() + " are now upgraded to ThirtyShotBot!");
                                ThirtyShotBot* upgradedRobot = new ThirtyShotBot(this->id(), this->x(), this->y());

                                // Copy state
                                upgradedRobot->setNumOfLives(this->numOfLives());
                                upgradedRobot->setNumOfKills(this->numOfKills());
                                // upgradedRobot->setNumOfShells(this->numOfShell());
                                upgradedRobot->setUpgradeCount(this->getUpgradeCount());

                                int robotName_pos = this->robotName().find("_");
                                upgradedRobot->setRobotName(upgradedRobot->id() + this->robotName().substr(robotName_pos));

                                // Replace the robot in the battlefield vector immediately
                                for (auto& robot : battlefield->robots()) {
                                    if (robot == this) {
                                        robot = upgradedRobot;
                                        break; // important: break to avoid double replacement
                                    }
                                }

                                return;
                            }
                            break;
                        }
                        case 3: {
                            // Randomly choose Seeing upgrade (1 - ScoutBot, 2 - TrackBot)
                            int seeChoice = rand() % 2 + 1;
                            if (seeChoice == 1) {
                                cout <<this->id() <<" are now upgraded to ScoutBot!"<< endl;
                                outputLines.push_back(this->id() + " are now upgraded to ScoutBot!");
                                ScoutBot* upgradedRobot = new ScoutBot(this->id(), this->x(), this->y());

                                // Copy state
                                upgradedRobot->setNumOfLives(this->numOfLives());
                                upgradedRobot->setNumOfKills(this->numOfKills());
                                upgradedRobot->setNumOfShells(this->numOfShell());
                                upgradedRobot->setUpgradeCount(this->getUpgradeCount());

                                int robotName_pos = this->robotName().find("_");
                                upgradedRobot->setRobotName(upgradedRobot->id() + this->robotName().substr(robotName_pos));

                                // Replace the robot in the battlefield vector immediately
                                for (auto& robot : battlefield->robots()) {
                                    if (robot == this) {
                                        robot = upgradedRobot;
                                        break; // important: break to avoid double replacement
                                    }
                                }

                                return;
                            } else {
                                cout <<this->id() <<" are now upgraded to TrackBot!"<< endl;
                                outputLines.push_back(this->id() + " are now upgraded to TrackBot!");
                                TrackBot* upgradedRobot = new TrackBot(this->id(), this->x(), this->y());

                                // Copy state
                                upgradedRobot->setNumOfLives(this->numOfLives());
                                upgradedRobot->setNumOfKills(this->numOfKills());
                                upgradedRobot->setNumOfShells(this->numOfShell());
                                upgradedRobot->setUpgradeCount(this->getUpgradeCount());

                                int robotName_pos = this->robotName().find("_");
                                upgradedRobot->setRobotName(upgradedRobot->id() + this->robotName().substr(robotName_pos));

                                // Replace the robot in the battlefield vector immediately
                                for (auto& robot : battlefield->robots()) {
                                    if (robot == this) {
                                        robot = upgradedRobot;
                                        break; // important: break to avoid double replacement
                                    }
                                }

                                return;
                            }
                            break;
                        }
                        case 4: {
                            // Randomly choose Extra upgrade (1 - HealthBot, 2 - GodBot, 3 - JukeBot)
                            int extraChoice = rand() % 3 + 1;
                            if (extraChoice == 1) {
                                cout << this->id() << " are now upgraded to HealthBot!" << endl;
                                outputLines.push_back(this->id() + " are now upgraded to HealthBot!");
                                HealthBot* upgradedRobot = new HealthBot(this->id(), this->x(), this->y());

                                // Copy state
                                upgradedRobot->setNumOfLives(this->numOfLives());
                                upgradedRobot->setNumOfKills(this->numOfKills());
                                upgradedRobot->setNumOfShells(this->numOfShell());
                                upgradedRobot->setUpgradeCount(this->getUpgradeCount());

                                int robotName_pos = this->robotName().find("_");
                                upgradedRobot->setRobotName(upgradedRobot->id() + this->robotName().substr(robotName_pos));

                                // Replace the robot in the battlefield vector immediately
                                for (auto& robot : battlefield->robots()) {
                                    if (robot == this) {
                                        robot = upgradedRobot;
                                        break; // important: break to avoid double replacement
                                    }
                                }

                                return;
                            } else if (extraChoice == 2) {
                                cout << this->id() << " are now upgraded to GodBot!" << endl;
                                outputLines.push_back(this->id() + " are now upgraded to GodBot!");
                                GodBot* upgradedRobot = new GodBot(this->id(), this->x(), this->y());

                                // Copy state
                                upgradedRobot->setNumOfLives(this->numOfLives());
                                upgradedRobot->setNumOfKills(this->numOfKills());
                                upgradedRobot->setNumOfShells(this->numOfShell());
                                upgradedRobot->setUpgradeCount(this->getUpgradeCount());

                                int robotName_pos = this->robotName().find("_");
                                upgradedRobot->setRobotName(upgradedRobot->id() + this->robotName().substr(robotName_pos));

                                // Replace the robot in the battlefield vector immediately
                                for (auto& robot : battlefield->robots()) {
                                    if (robot == this) {
                                        robot = upgradedRobot;
                                        break; // important: break to avoid double replacement
                                    }
                                }

                                return;
                            } else {
                                cout <<this->id() <<" are now upgraded to JukeBot!"<< endl;
                                outputLines.push_back(this->id() + " are now upgraded to JukeBot!");
                                JukeBot* upgradedRobot = new JukeBot(this->id(), this->x(), this->y());

                                // Copy state
                                upgradedRobot->setNumOfLives(this->numOfLives());
                                upgradedRobot->setNumOfKills(this->numOfKills());
                                upgradedRobot->setNumOfShells(this->numOfShell());
                                upgradedRobot->setUpgradeCount(this->getUpgradeCount());

                                int robotName_pos = this->robotName().find("_");
                                upgradedRobot->setRobotName(upgradedRobot->id() + this->robotName().substr(robotName_pos));

                                // Replace the robot in the battlefield vector immediately
                                for (auto& robot : battlefield->robots()) {
                                    if (robot == this) {
                                        robot = upgradedRobot;
                                        break; // important: break to avoid double replacement
                                    }
                                }

                                return;
                            }
                            break;
                        }
                    }

                } else {
                    cout << "Upgrade limit reached. Can only upgrade twice." << endl;
                    outputLines.push_back(this->id() + " are now upgraded to JukeBot!");
                }
            } else {
                this->decreaseShell();
                cout<<"Shot missed! The enemy robot was not hit."<<endl;
                cout<< this->id() <<" now has "<< this->numOfShell() <<" of shells left!"<<endl;

                outputLines.push_back("Shot missed! The enemy robot was not hit.");
                outputLines.push_back(this->id() + " now has " + to_string(this->numOfShell()) + " of shells left!");
            }
            hit = true;
        }
    }

    if (!hit) {
        this->decreaseShell();
        cout<<"No enemy robot was at the selected location."<<endl;
        cout<< this->id() <<" now has "<< this->numOfShell() <<" of shells left!"<<endl;

        outputLines.push_back("No enemy robot was at the selected location.");
        outputLines.push_back(this->id() + " now has " + to_string(this->numOfShell()) + " of shells left!");

    }
}


// ScoutBot Action Logics
void ScoutBot::actionThink(Battlefield* battlefield) {
    cout << "ScoutBot is analyzing the battlefield..." << endl;
    outputLines.push_back("ScoutBot is analyzing the battlefield...");
}

void ScoutBot::actionLook(Battlefield* battlefield) {
    int currentX = robotPosX;
    int currentY = robotPosY;

    cout << "=== Robot View (3x3 Grid) ===" << endl;
    outputLines.push_back("=== Robot View (3x3 Grid) ===");

    for (int dy = -1; dy <= 1; ++dy) {
        string row = "";
        for (int dx = -1; dx <= 1; ++dx) {
            int x = currentX + dx;
            int y = currentY + dy;

            if (y >= 0 && y < battlefield->BATTLEFIELD_NUM_OF_ROWS() &&
                x >= 0 && x < battlefield->BATTLEFIELD_NUM_OF_COLS()) {

                if (x == currentX && y == currentY) {
                    cout << "[R] ";
                    row += "[R] ";
                } else {
                    string content = battlefield->getCellContent(x, y);
                    if (content.empty()) {
                        cout << "[ ] ";
                        row += "[ ] ";
                    } else {
                        cout << "[" << content.substr(0, 1) << "] ";
                        row += "[" + content.substr(0, 1) + "] ";
                    }
                }
            } else {
                cout << "[#] ";
                row += "[#] ";
            }
        }
        cout << endl;
        outputLines.push_back(row);
    }


    int LookChoice = rand() % 2;

    if (LookChoice == 0 && LookAllCount != 0){
        cout<<"ScoutBot decided to use Look All"<<endl;
        outputLines.push_back("ScoutBot decided to use Look All");
        LookAllCount--;
        Showallbot(battlefield);
    } else {
        cout<<"ScoutBot decided not to use Look All"<<endl;
        outputLines.push_back("ScoutBot decided not to use Look All");
    }


}

void ScoutBot::Showallbot(Battlefield* battlefield) {
        cout << "Display ALL Robots in Battlefield" << endl;
        outputLines.push_back("Display ALL Robots in Battlefield");


        cout << endl << "     ";
        string header = "     ";
        for (int j = 0; j < (battlefield->returnbattlefield_())[0].size(); j++) {
            cout << "  " << right << setfill('0') << setw(2) << j << " ";
            ostringstream oss;
            oss << "  " << right << setfill('0') << setw(2) << j << " ";
            header += oss.str();
        }
        cout << endl;
        outputLines.push_back(header);

        // Loop through each row
        for (int i = 0; i < (battlefield->returnbattlefield_()).size(); i++) {
            // Top border line
            cout << "   ";
            string topLine = "   ";
            for (int j = 0; j < (battlefield->returnbattlefield_())[i].size(); j++) {
                cout << "+----";
                topLine += "+----";
            }
            cout << "+" << endl;
            topLine += "+";
            outputLines.push_back(topLine);

            // Content line with robot IDs or empty cells
            cout << " " << right << setfill('0') << setw(2) << i;
            ostringstream rowLine;
            rowLine << " " << right << setfill('0') << setw(2) << i;

            for (int j = 0; j < (battlefield->returnbattlefield_())[i].size(); j++) {
                string cellContent = (battlefield->returnbattlefield_())[i][j];
                if (cellContent == "") {
                    cout << "|    ";
                    rowLine << "|    ";
                } else {
                    cout << "|" << left << setfill(' ') << setw(4) << cellContent;
                    rowLine << "|" << left << setfill(' ') << setw(4) << cellContent;
                }
            }
            cout << "|" << endl;
            rowLine << "|";
            outputLines.push_back(rowLine.str());
        }

        // Bottom border line
        cout << "   ";
        string bottomLine = "   ";
        for (int j = 0; j < (battlefield->returnbattlefield_())[0].size(); j++) {
            cout << "+----";
            bottomLine += "+----";
        }
        cout << "+" << endl;
        bottomLine += "+";
        outputLines.push_back(bottomLine);
}

void ScoutBot::actionMove(Battlefield* battlefield) {
    int currentX = robotPosX;
    int currentY = robotPosY;

    vector<string> directions = {"UL", "U", "UR", "L", "C", "R", "DL", "D", "DR"};
    vector<pair<int, int>> directionsMove = {
        {-1, -1}, {0, -1}, {1, -1},
        {-1, 0},  {0, 0},  {1, 0},
        {-1, 1},  {0, 1},  {1, 1}
    };

    cout << "\nAvailable Directions:\n";
    cout << "UL, U, UR\n";
    cout << "L, C, R\n";
    cout << "DL, D, DR\n";

    outputLines.push_back("\nAvailable Directions:");
    outputLines.push_back("UL, U, UR");
    outputLines.push_back("L, C, R");
    outputLines.push_back("DL, D, DR");

    // store valid index of move location  into validIndex
    vector<int> validIndex;
    for (int i = 0; i < directions.size(); ++i) {
        int newX = currentX + directionsMove[i].first;
        int newY = currentY + directionsMove[i].second;

        // Check bounds
        if (newX < 0 || newX >= battlefield->BATTLEFIELD_NUM_OF_COLS() ||
            newY < 0 || newY >= battlefield->BATTLEFIELD_NUM_OF_ROWS())
            continue;

        // Check if destination is occupied
        bool isOccupied = false;
        for (Robot* robot : battlefield->robots()) {
            if (robot->x() == newX && robot->y() == newY) {
                isOccupied = true;
                break;
            }
        }

        if (!isOccupied) {
            validIndex.push_back(i); // push back the index of the valid move
        }
    }

    // Handle case when no valid directions
    if (validIndex.empty()) {
        cout << "No valid move directions available\n";
        outputLines.push_back("No valid move directions available");
        return;
    }

    // Randomly pick a valid direction
    int chosenIndex = validIndex[rand() % validIndex.size()];
    string userInput = directions[chosenIndex];

    cout << "Random direction selected: " << userInput << endl;
    outputLines.push_back("Random direction selected: " + userInput);

    // Calculate new position
    int newX = currentX + directionsMove[chosenIndex].first;
    int newY = currentY + directionsMove[chosenIndex].second;

    // Update position
    robotPosX = newX;
    robotPosY = newY;
    battlefield->setCell(currentX, currentY, nullptr);
    battlefield->setCell(newX, newY, this);

    cout << "Moved to (" << newX << "," << newY << ")" << endl;
    outputLines.push_back("Moved to (" + to_string(newX) + "," + to_string(newY) + ")");
}

void ScoutBot::actionShoot(Battlefield* battlefield){

    string CurrentRobotsName = this->robotName();
    cout << "The Current Robot controlled is " << CurrentRobotsName << endl;
    outputLines.push_back("The Current Robot controlled is " + CurrentRobotsName);

    int CurrentRobotsX = this->x();
    int CurrentRobotsY = this->y();

    int targetX, targetY;
    bool validTarget = false;

    int battlefieldWidth = battlefield->BATTLEFIELD_NUM_OF_COLS();
    int battlefieldHeight = battlefield->BATTLEFIELD_NUM_OF_ROWS();

    // Generate list of valid adjacent targets
    vector<pair<int, int>> possibleTargets;
    for (int dx = -1; dx <= 1; dx++) {
        for (int dy = -1; dy <= 1; dy++) {
            if (dx == 0 && dy == 0) continue; // Skip self

            int newX = CurrentRobotsX + dx;
            int newY = CurrentRobotsY + dy;

            // Check if within bounds
            if (newX >= 0 && newX < battlefieldWidth && newY >= 0 && newY < battlefieldHeight) {
                possibleTargets.emplace_back(newX, newY);
            }
        }
    }

    if (!possibleTargets.empty()) {
        int index = rand() % possibleTargets.size();
        targetX = possibleTargets[index].first;
        targetY = possibleTargets[index].second;

        cout << "Random target selected at: " << targetX << " " << targetY << endl;
        outputLines.push_back("Random target selected at: " + to_string(targetX) + " " + to_string(targetY));

        validTarget = true;
    } else {
        cout << "No valid adjacent tiles found for targeting." << endl;
        outputLines.push_back("No valid adjacent tiles found for targeting.");
        return;
    }

    // Shell check
    int ShellLeft = this->numOfShell();
    if (ShellLeft == 0) {
        cout << "Oh no! You ran out of shells! You can't shoot anything! YOU ARE SELF DESTRUCTING!" << endl;
        outputLines.push_back("Oh no! You ran out of shells! You can't shoot anything! YOU ARE SELF DESTRUCTING!");
        this->reduceLives();
        this->setNumOfShells(10);

        return;
    }

    bool hit = false;

    for (Robot* robot : battlefield->robots()) {
        string targetRobotId = robot->id() ;
        int PotentialRobotX = robot->x() ;
        int PotentialRobotY = robot->y() ;
        if (targetX == CurrentRobotsX && targetY == CurrentRobotsY ){
            cout<<"\nYou can't Shoot Yourself"<<endl;
            outputLines.push_back("\nYou can't Shoot Yourself");
            break;
        }
        if (targetX == PotentialRobotX && targetY == PotentialRobotY ){
            if (!robot->canBeTargeted()) {
                cout << "Target is hidden and cannot be hit!" << endl;
                outputLines.push_back("Target is hidden and cannot be hit!");
                return;
            }
            int hitChance = rand() % 100; // number from 0-99
            if (hitChance < 70) { // 70% chance to hit
                cout<<"\nYou've successfully shot an enemy Robot!"<<endl;
                outputLines.push_back("\nYou've successfully shot an enemy Robot!");
                robot->reduceLives();

                if (!robot->isAlive()) {
                    cout << targetRobotId << " has been destroyed!" << endl;
                    outputLines.push_back(targetRobotId + " has been destroyed!");

                    battlefield->setCell(PotentialRobotX, PotentialRobotY, nullptr); // remove robot from battlefield
                    battlefield->clearCell(PotentialRobotX, PotentialRobotY);
                    robot->setX(-1);
                    robot->setY(-2);

                    battlefield->destroyedRobots().push(robot);  // add to destroyed queue

                    // // Optional: If destroyed robots should be removed from active list
                    // auto& robotsVec = battlefield->robots();
                    // auto it = find(robotsVec.begin(), robotsVec.end(), robot);
                    // if (it != robotsVec.end()) {
                    //     robotsVec.erase(it);
                    // }

                } else {
                    cout << targetRobotId << " was hit and is now temporarily inactive!" << endl;
                    outputLines.push_back(targetRobotId + " was hit and is now temporarily inactive!");

                    battlefield->setCell(PotentialRobotX, PotentialRobotY, nullptr); // remove temporarily
                    battlefield->clearCell(PotentialRobotX, PotentialRobotY);
                    robot->setX(-1);
                    robot->setY(-1);

                    battlefield->waitingRobots().push({robot, 0}); // add to waiting queue

                    // auto& robotsVec = battlefield->robots();
                    // auto it = find(robotsVec.begin(), robotsVec.end(), robot);
                    // if (it != robotsVec.end()) {
                    //     robotsVec.erase(it);
                    // }
                }


                this->increaseKills();
                this->decreaseShell();
                int lifeLeft = robot->numOfLives();

                cout<< targetRobotId<<" now has "<<lifeLeft<<" of lives left"<<endl;
                cout<< this->id() <<" now has "<< this->numOfKills() <<" of kills!"<<endl;
                cout<< this->id() <<" now has "<< this->numOfShell() <<" of shells left!"<<endl;

                outputLines.push_back(targetRobotId + " now has " + to_string(lifeLeft) + " of lives left");
                outputLines.push_back(this->id() + " now has " + to_string(this->numOfKills()) + " of kills!");
                outputLines.push_back(this->id() + " now has " + to_string(this->numOfShell()) + " of shells left!");

                // cout << "Remaining Robots: " << endl;
                // for (const auto& r : battlefield->robots()) {
                //     cout << *r << endl;
                // }


                if (this->canUpgrade()) {
                    this->incrementUpgradeCount();

                    cout<<"\n" << this->id() <<" earned an upgrade! A random upgrade is applied!" << endl;
                    outputLines.push_back("\n" + this->id() + " earned an upgrade! A random upgrade is applied!");
                    //cout <<this->id() <<" are now upgraded to ScoutBot!"<< endl;
                    // ScoutBot* upgradedRobot = new ScoutBot(this->id().substr(5), this->x(), this->y());
                    // IM LOSING MY SHIT HERE

                    //Randomly choose upgrade category (1 - Moving, 2 - Shooting, 3 - Extras)
                    int choice = rand() % 3 + 1;

                    switch (choice) {
                        case 1: {
                            // Randomly choose Moving upgrade (1 - HideBot, 2 - JumpBot)
                            int moveChoice = rand() % 2 + 1;
                            if (moveChoice == 1) {
                                cout <<this->id() <<" are now upgraded to HideBot!"<< endl;
                                outputLines.push_back(this->id() + " are now upgraded to HideBot!");

                                HideBot* upgradedRobot = new HideBot(this->id(), this->x(), this->y());

                                // Copy state
                                upgradedRobot->setNumOfLives(this->numOfLives());
                                upgradedRobot->setNumOfKills(this->numOfKills());
                                upgradedRobot->setNumOfShells(this->numOfShell());
                                upgradedRobot->setUpgradeCount(this->getUpgradeCount());

                                int robotName_pos = this->robotName().find("_");
                                upgradedRobot->setRobotName(upgradedRobot->id() + this->robotName().substr(robotName_pos));

                                // Replace the robot in the battlefield vector immediately
                                for (auto& robot : battlefield->robots()) {
                                    if (robot == this) {
                                        robot = upgradedRobot;
                                        break; // important: break to avoid double replacement
                                    }
                                }

                                return;
                            } else {
                                cout <<this->id() <<" are now upgraded to JumpBot!"<< endl;
                                outputLines.push_back(this->id() + " are now upgraded to JumpBot!");
                                JumpBot* upgradedRobot = new JumpBot(this->id(), this->x(), this->y());

                                // Copy state
                                upgradedRobot->setNumOfLives(this->numOfLives());
                                upgradedRobot->setNumOfKills(this->numOfKills());
                                upgradedRobot->setNumOfShells(this->numOfShell());
                                upgradedRobot->setUpgradeCount(this->getUpgradeCount());

                                int robotName_pos = this->robotName().find("_");
                                upgradedRobot->setRobotName(upgradedRobot->id() + this->robotName().substr(robotName_pos));

                                // Replace the robot in the battlefield vector immediately
                                for (auto& robot : battlefield->robots()) {
                                    if (robot == this) {
                                        robot = upgradedRobot;
                                        break; // important: break to avoid double replacement
                                    }
                                }

                                return;
                            }
                        }
                        case 2: {
                            // Randomly choose Shooting upgrade (1 - LongShotBot, 2 - SemiAutoBot, 3 - ThirtyShotBot)
                            int shootChoice = rand() % 3 + 1;
                            if (shootChoice == 1) {
                                cout <<this->id() <<" are now upgraded to LongShotBot!"<< endl;
                                outputLines.push_back(this->id() + " are now upgraded to LongShotBot!");
                                LongShotBot* upgradedRobot = new LongShotBot(this->id(), this->x(), this->y());

                                // Copy state
                                upgradedRobot->setNumOfLives(this->numOfLives());
                                upgradedRobot->setNumOfKills(this->numOfKills());
                                upgradedRobot->setNumOfShells(this->numOfShell());
                                upgradedRobot->setUpgradeCount(this->getUpgradeCount());

                                int robotName_pos = this->robotName().find("_");
                                upgradedRobot->setRobotName(upgradedRobot->id() + this->robotName().substr(robotName_pos));

                                // Replace the robot in the battlefield vector immediately
                                for (auto& robot : battlefield->robots()) {
                                    if (robot == this) {
                                        robot = upgradedRobot;
                                        break; // important: break to avoid double replacement
                                    }
                                }

                                return;
                            } else if (shootChoice == 2) {
                                cout <<this->id() <<" are now upgraded to SemiAutoBot!"<< endl;
                                outputLines.push_back(this->id() + " are now upgraded to SemiAutoBot!");
                                SemiAutoBot* upgradedRobot = new SemiAutoBot(this->id(), this->x(), this->y());

                                // Copy state
                                upgradedRobot->setNumOfLives(this->numOfLives());
                                upgradedRobot->setNumOfKills(this->numOfKills());
                                upgradedRobot->setNumOfShells(this->numOfShell());
                                upgradedRobot->setUpgradeCount(this->getUpgradeCount());

                                int robotName_pos = this->robotName().find("_");
                                upgradedRobot->setRobotName(upgradedRobot->id() + this->robotName().substr(robotName_pos));

                                // Replace the robot in the battlefield vector immediately
                                for (auto& robot : battlefield->robots()) {
                                    if (robot == this) {
                                        robot = upgradedRobot;
                                        break; // important: break to avoid double replacement
                                    }
                                }

                                return;
                            } else {
                                cout <<this->id() <<" are now upgraded to ThirtyShotBot!"<< endl;
                                outputLines.push_back(this->id() + " are now upgraded to ThirtyShotBot!");
                                ThirtyShotBot* upgradedRobot = new ThirtyShotBot(this->id(), this->x(), this->y());

                                // Copy state
                                upgradedRobot->setNumOfLives(this->numOfLives());
                                upgradedRobot->setNumOfKills(this->numOfKills());
                                // upgradedRobot->setNumOfShells(this->numOfShell());
                                upgradedRobot->setUpgradeCount(this->getUpgradeCount());

                                int robotName_pos = this->robotName().find("_");
                                upgradedRobot->setRobotName(upgradedRobot->id() + this->robotName().substr(robotName_pos));

                                // Replace the robot in the battlefield vector immediately
                                for (auto& robot : battlefield->robots()) {
                                    if (robot == this) {
                                        robot = upgradedRobot;
                                        break; // important: break to avoid double replacement
                                    }
                                }

                                return;
                            }
                            break;
                        }
                        // case 3: {
                        //     // Randomly choose Seeing upgrade (1 - ScoutBot, 2 - TrackBot)
                        //     int seeChoice = rand() % 2 + 1;
                        //     if (seeChoice == 1) {
                        //         cout <<this->id() <<" are now upgraded to ScoutBot!"<< endl;
                        //         outputLines.push_back(this->id() + " are now upgraded to ScoutBot!");
                        //         ScoutBot* upgradedRobot = new ScoutBot(this->id(), this->x(), this->y());

                        //         // Copy state
                        //         upgradedRobot->setNumOfLives(this->numOfLives());
                        //         upgradedRobot->setNumOfKills(this->numOfKills());
                        //         upgradedRobot->setNumOfShells(this->numOfShell());
                        //         upgradedRobot->setUpgradeCount(this->getUpgradeCount());

                        //         int robotName_pos = this->robotName().find("_");
                        //         upgradedRobot->setRobotName(upgradedRobot->id() + this->robotName().substr(robotName_pos));

                        //         // Replace the robot in the battlefield vector immediately
                        //         for (auto& robot : battlefield->robots()) {
                        //             if (robot == this) {
                        //                 robot = upgradedRobot;
                        //                 break; // important: break to avoid double replacement
                        //             }
                        //         }

                        //         return;
                        //     } else {
                        //         cout <<this->id() <<" are now upgraded to TrackBot!"<< endl;
                        //         outputLines.push_back(this->id() + " are now upgraded to TrackBot!");
                        //         TrackBot* upgradedRobot = new TrackBot(this->id(), this->x(), this->y());

                        //         // Copy state
                        //         upgradedRobot->setNumOfLives(this->numOfLives());
                        //         upgradedRobot->setNumOfKills(this->numOfKills());
                        //         upgradedRobot->setNumOfShells(this->numOfShell());
                        //         upgradedRobot->setUpgradeCount(this->getUpgradeCount());

                        //         int robotName_pos = this->robotName().find("_");
                        //         upgradedRobot->setRobotName(upgradedRobot->id() + this->robotName().substr(robotName_pos));

                        //         // Replace the robot in the battlefield vector immediately
                        //         for (auto& robot : battlefield->robots()) {
                        //             if (robot == this) {
                        //                 robot = upgradedRobot;
                        //                 break; // important: break to avoid double replacement
                        //             }
                        //         }

                        //         return;
                        //     }
                        //     break;
                        // }
                        case 3: {
                            // Randomly choose Extra upgrade (1 - HealthBot, 2 - GodBot, 3 - JukeBot)
                            int extraChoice = rand() % 3 + 1;
                            if (extraChoice == 1) {
                                cout << this->id() << " are now upgraded to HealthBot!" << endl;
                                outputLines.push_back(this->id() + " are now upgraded to HealthBot!");
                                HealthBot* upgradedRobot = new HealthBot(this->id(), this->x(), this->y());

                                // Copy state
                                upgradedRobot->setNumOfLives(this->numOfLives());
                                upgradedRobot->setNumOfKills(this->numOfKills());
                                upgradedRobot->setNumOfShells(this->numOfShell());
                                upgradedRobot->setUpgradeCount(this->getUpgradeCount());

                                int robotName_pos = this->robotName().find("_");
                                upgradedRobot->setRobotName(upgradedRobot->id() + this->robotName().substr(robotName_pos));

                                // Replace the robot in the battlefield vector immediately
                                for (auto& robot : battlefield->robots()) {
                                    if (robot == this) {
                                        robot = upgradedRobot;
                                        break; // important: break to avoid double replacement
                                    }
                                }

                                return;
                            } else if (extraChoice == 2) {
                                cout << this->id() << " are now upgraded to GodBot!" << endl;
                                outputLines.push_back(this->id() + " are now upgraded to GodBot!");
                                GodBot* upgradedRobot = new GodBot(this->id(), this->x(), this->y());

                                // Copy state
                                upgradedRobot->setNumOfLives(this->numOfLives());
                                upgradedRobot->setNumOfKills(this->numOfKills());
                                upgradedRobot->setNumOfShells(this->numOfShell());
                                upgradedRobot->setUpgradeCount(this->getUpgradeCount());

                                int robotName_pos = this->robotName().find("_");
                                upgradedRobot->setRobotName(upgradedRobot->id() + this->robotName().substr(robotName_pos));

                                // Replace the robot in the battlefield vector immediately
                                for (auto& robot : battlefield->robots()) {
                                    if (robot == this) {
                                        robot = upgradedRobot;
                                        break; // important: break to avoid double replacement
                                    }
                                }

                                return;
                            } else {
                                cout <<this->id() <<" are now upgraded to JukeBot!"<< endl;
                                outputLines.push_back(this->id() + " are now upgraded to JukeBot!");
                                JukeBot* upgradedRobot = new JukeBot(this->id(), this->x(), this->y());

                                // Copy state
                                upgradedRobot->setNumOfLives(this->numOfLives());
                                upgradedRobot->setNumOfKills(this->numOfKills());
                                upgradedRobot->setNumOfShells(this->numOfShell());
                                upgradedRobot->setUpgradeCount(this->getUpgradeCount());

                                int robotName_pos = this->robotName().find("_");
                                upgradedRobot->setRobotName(upgradedRobot->id() + this->robotName().substr(robotName_pos));

                                // Replace the robot in the battlefield vector immediately
                                for (auto& robot : battlefield->robots()) {
                                    if (robot == this) {
                                        robot = upgradedRobot;
                                        break; // important: break to avoid double replacement
                                    }
                                }

                                return;
                            }
                            break;
                        }
                    }

                } else {
                    cout << "Upgrade limit reached. Can only upgrade twice." << endl;
                    outputLines.push_back(this->id() + " are now upgraded to JukeBot!");
                }
            } else {
                this->decreaseShell();
                cout<<"Shot missed! The enemy robot was not hit."<<endl;
                cout<< this->id() <<" now has "<< this->numOfShell() <<" of shells left!"<<endl;

                outputLines.push_back("Shot missed! The enemy robot was not hit.");
                outputLines.push_back(this->id() + " now has " + to_string(this->numOfShell()) + " of shells left!");
            }
            hit = true;
        }
    }

    if (!hit) {
        this->decreaseShell();
        cout<<"No enemy robot was at the selected location."<<endl;
        cout<< this->id() <<" now has "<< this->numOfShell() <<" of shells left!"<<endl;

        outputLines.push_back("No enemy robot was at the selected location.");
        outputLines.push_back(this->id() + " now has " + to_string(this->numOfShell()) + " of shells left!");

    }
}


//TrackBot Action Logics
void TrackBot::actionThink(Battlefield* battlefield) {
    cout << "Trackbot is analyzing the battlefield..." << endl;
    outputLines.push_back("Trackbot is analyzing the battlefield...");


    // Track nearby robots
    if (trackedRobots_.empty()) {
        cout << "No robots are being tracked." << endl;
        outputLines.push_back("No robots are being tracked.");

    } else {
        cout << "Tracked Robots:" << endl;
        outputLines.push_back("Tracked Robots:");

        for (const auto& tracked : trackedRobots_) {
            Robot* robot = tracked;
            cout << " - " << robot->id() << " at (" << robot->x() << ", " << robot->y() << ")" << endl;
            outputLines.push_back(" - " + robot->id() + " at (" + to_string(robot->x()) + ", "+ to_string(robot->y()) + ")");

        }
    }
}

void TrackBot::actionLook(Battlefield* battlefield) {
    int currentX = robotPosX;
    int currentY = robotPosY;

    cout << "=== Robot View (3x3 Grid) ===" << endl;
    outputLines.push_back("=== Robot View (3x3 Grid) ===");

    for (int dy = -1; dy <= 1; ++dy) {
        string row = "";
        for (int dx = -1; dx <= 1; ++dx) {
            int x = currentX + dx;
            int y = currentY + dy;

            if (y >= 0 && y < battlefield->BATTLEFIELD_NUM_OF_ROWS() &&
                x >= 0 && x < battlefield->BATTLEFIELD_NUM_OF_COLS()) {

                if (x == currentX && y == currentY) {
                    cout << "[R] ";
                    row += "[R] ";
                } else {
                    string content = battlefield->getCellContent(x, y);
                    if (content.empty()) {
                        cout << "[ ] ";
                        row += "[ ] ";
                    } else {
                        cout << "[" << content.substr(0, 1) << "] ";
                        row += "[" + content.substr(0, 1) + "] ";
                        // Find the Robot* at (x, y) and track it
                        for (Robot* robot : battlefield->robots()) {
                            if (robot->x() == x && robot->y() == y) {
                                trackedRobots_.push_back(robot);
                                break;
                            }
                        }
                    }
                }
            } else {
                cout << "[#] ";
                row += "[#] ";
            }
        }
        cout << endl;
        outputLines.push_back(row);
    }
}

void TrackBot::actionMove(Battlefield* battlefield) {
    int currentX = robotPosX;
    int currentY = robotPosY;

    vector<string> directions = {"UL", "U", "UR", "L", "C", "R", "DL", "D", "DR"};
    vector<pair<int, int>> directionsMove = {
        {-1, -1}, {0, -1}, {1, -1},
        {-1, 0},  {0, 0},  {1, 0},
        {-1, 1},  {0, 1},  {1, 1}
    };

    cout << "\nAvailable Directions:\n";
    cout << "UL, U, UR\n";
    cout << "L, C, R\n";
    cout << "DL, D, DR\n";

    outputLines.push_back("\nAvailable Directions:");
    outputLines.push_back("UL, U, UR");
    outputLines.push_back("L, C, R");
    outputLines.push_back("DL, D, DR");

    // store valid index of move location  into validIndex
    vector<int> validIndex;
    for (int i = 0; i < directions.size(); ++i) {
        int newX = currentX + directionsMove[i].first;
        int newY = currentY + directionsMove[i].second;

        // Check bounds
        if (newX < 0 || newX >= battlefield->BATTLEFIELD_NUM_OF_COLS() ||
            newY < 0 || newY >= battlefield->BATTLEFIELD_NUM_OF_ROWS())
            continue;

        // Check if destination is occupied
        bool isOccupied = false;
        for (Robot* robot : battlefield->robots()) {
            if (robot->x() == newX && robot->y() == newY) {
                isOccupied = true;
                break;
            }
        }

        if (!isOccupied) {
            validIndex.push_back(i); // push back the index of the valid move
        }
    }

    // Handle case when no valid directions
    if (validIndex.empty()) {
        cout << "No valid move directions available\n";
        outputLines.push_back("No valid move directions available");
        return;
    }

    // Randomly pick a valid direction
    int chosenIndex = validIndex[rand() % validIndex.size()];
    string userInput = directions[chosenIndex];

    cout << "Random direction selected: " << userInput << endl;
    outputLines.push_back("Random direction selected: " + userInput);

    // Calculate new position
    int newX = currentX + directionsMove[chosenIndex].first;
    int newY = currentY + directionsMove[chosenIndex].second;

    // Update position
    robotPosX = newX;
    robotPosY = newY;
    battlefield->setCell(currentX, currentY, nullptr);
    battlefield->setCell(newX, newY, this);

    cout << "Moved to (" << newX << "," << newY << ")" << endl;
    outputLines.push_back("Moved to (" + to_string(newX) + "," + to_string(newY) + ")");
}

void TrackBot::actionShoot(Battlefield* battlefield) {

    string CurrentRobotsName = this->robotName();
    cout << "The Current Robot controlled is " << CurrentRobotsName << endl;
    outputLines.push_back("The Current Robot controlled is " + CurrentRobotsName);

    int CurrentRobotsX = this->x();
    int CurrentRobotsY = this->y();

    int targetX, targetY;
    bool validTarget = false;

    int battlefieldWidth = battlefield->BATTLEFIELD_NUM_OF_COLS();
    int battlefieldHeight = battlefield->BATTLEFIELD_NUM_OF_ROWS();

    // Generate list of valid adjacent targets
    vector<pair<int, int>> possibleTargets;
    for (int dx = -1; dx <= 1; dx++) {
        for (int dy = -1; dy <= 1; dy++) {
            if (dx == 0 && dy == 0) continue; // Skip self

            int newX = CurrentRobotsX + dx;
            int newY = CurrentRobotsY + dy;

            // Check if within bounds
            if (newX >= 0 && newX < battlefieldWidth && newY >= 0 && newY < battlefieldHeight) {
                possibleTargets.emplace_back(newX, newY);
            }
        }
    }

    if (!possibleTargets.empty()) {
        int index = rand() % possibleTargets.size();
        targetX = possibleTargets[index].first;
        targetY = possibleTargets[index].second;

        cout << "Random target selected at: " << targetX << " " << targetY << endl;
        outputLines.push_back("Random target selected at: " + to_string(targetX) + " " + to_string(targetY));

        validTarget = true;
    } else {
        cout << "No valid adjacent tiles found for targeting." << endl;
        outputLines.push_back("No valid adjacent tiles found for targeting.");
        return;
    }

    // Shell check
    int ShellLeft = this->numOfShell();
    if (ShellLeft == 0) {
        cout << "Oh no! You ran out of shells! You can't shoot anything! YOU ARE SELF DESTRUCTING!" << endl;
        outputLines.push_back("Oh no! You ran out of shells! You can't shoot anything! YOU ARE SELF DESTRUCTING!");
        this->reduceLives();
        this->setNumOfShells(10);

        return;
    }

    bool hit = false;

    for (Robot* robot : battlefield->robots()) {
        string targetRobotId = robot->id() ;
        int PotentialRobotX = robot->x() ;
        int PotentialRobotY = robot->y() ;
        if (targetX == CurrentRobotsX && targetY == CurrentRobotsY ){
            cout<<"\nYou can't Shoot Yourself"<<endl;
            outputLines.push_back("\nYou can't Shoot Yourself");
            break;
        }
        if (targetX == PotentialRobotX && targetY == PotentialRobotY ){
            if (!robot->canBeTargeted()) {
                cout << "Target is hidden and cannot be hit!" << endl;
                outputLines.push_back("Target is hidden and cannot be hit!");
                return;
            }
            int hitChance = rand() % 100; // number from 0-99
            if (hitChance < 70) { // 70% chance to hit
                cout<<"\nYou've successfully shot an enemy Robot!"<<endl;
                outputLines.push_back("\nYou've successfully shot an enemy Robot!");
                robot->reduceLives();

                if (!robot->isAlive()) {
                    cout << targetRobotId << " has been destroyed!" << endl;
                    outputLines.push_back(targetRobotId + " has been destroyed!");

                    battlefield->setCell(PotentialRobotX, PotentialRobotY, nullptr); // remove robot from battlefield
                    battlefield->clearCell(PotentialRobotX, PotentialRobotY);
                    robot->setX(-1);
                    robot->setY(-2);

                    battlefield->destroyedRobots().push(robot);  // add to destroyed queue

                    // // Optional: If destroyed robots should be removed from active list
                    // auto& robotsVec = battlefield->robots();
                    // auto it = find(robotsVec.begin(), robotsVec.end(), robot);
                    // if (it != robotsVec.end()) {
                    //     robotsVec.erase(it);
                    // }

                } else {
                    cout << targetRobotId << " was hit and is now temporarily inactive!" << endl;
                    outputLines.push_back(targetRobotId + " was hit and is now temporarily inactive!");

                    battlefield->setCell(PotentialRobotX, PotentialRobotY, nullptr); // remove temporarily
                    battlefield->clearCell(PotentialRobotX, PotentialRobotY);
                    robot->setX(-1);
                    robot->setY(-1);

                    battlefield->waitingRobots().push({robot, 0}); // add to waiting queue

                    // auto& robotsVec = battlefield->robots();
                    // auto it = find(robotsVec.begin(), robotsVec.end(), robot);
                    // if (it != robotsVec.end()) {
                    //     robotsVec.erase(it);
                    // }
                }


                this->increaseKills();
                this->decreaseShell();
                int lifeLeft = robot->numOfLives();

                cout<< targetRobotId<<" now has "<<lifeLeft<<" of lives left"<<endl;
                cout<< this->id() <<" now has "<< this->numOfKills() <<" of kills!"<<endl;
                cout<< this->id() <<" now has "<< this->numOfShell() <<" of shells left!"<<endl;

                outputLines.push_back(targetRobotId + " now has " + to_string(lifeLeft) + " of lives left");
                outputLines.push_back(this->id() + " now has " + to_string(this->numOfKills()) + " of kills!");
                outputLines.push_back(this->id() + " now has " + to_string(this->numOfShell()) + " of shells left!");

                // cout << "Remaining Robots: " << endl;
                // for (const auto& r : battlefield->robots()) {
                //     cout << *r << endl;
                // }


                if (this->canUpgrade()) {
                    this->incrementUpgradeCount();

                    cout<<"\n" << this->id() <<" earned an upgrade! A random upgrade is applied!" << endl;
                    outputLines.push_back("\n" + this->id() + " earned an upgrade! A random upgrade is applied!");
                    //cout <<this->id() <<" are now upgraded to ScoutBot!"<< endl;
                    // ScoutBot* upgradedRobot = new ScoutBot(this->id().substr(5), this->x(), this->y());
                    // IM LOSING MY SHIT HERE

                    //Randomly choose upgrade category (1 - Moving, 2 - Shooting, 3 - Extras)
                    int choice = rand() % 3 + 1;

                    switch (choice) {
                        case 1: {
                            // Randomly choose Moving upgrade (1 - HideBot, 2 - JumpBot)
                            int moveChoice = rand() % 2 + 1;
                            if (moveChoice == 1) {
                                cout <<this->id() <<" are now upgraded to HideBot!"<< endl;
                                outputLines.push_back(this->id() + " are now upgraded to HideBot!");

                                HideBot* upgradedRobot = new HideBot(this->id(), this->x(), this->y());

                                // Copy state
                                upgradedRobot->setNumOfLives(this->numOfLives());
                                upgradedRobot->setNumOfKills(this->numOfKills());
                                upgradedRobot->setNumOfShells(this->numOfShell());
                                upgradedRobot->setUpgradeCount(this->getUpgradeCount());

                                int robotName_pos = this->robotName().find("_");
                                upgradedRobot->setRobotName(upgradedRobot->id() + this->robotName().substr(robotName_pos));

                                // Replace the robot in the battlefield vector immediately
                                for (auto& robot : battlefield->robots()) {
                                    if (robot == this) {
                                        robot = upgradedRobot;
                                        break; // important: break to avoid double replacement
                                    }
                                }

                                return;
                            } else {
                                cout <<this->id() <<" are now upgraded to JumpBot!"<< endl;
                                outputLines.push_back(this->id() + " are now upgraded to JumpBot!");
                                JumpBot* upgradedRobot = new JumpBot(this->id(), this->x(), this->y());

                                // Copy state
                                upgradedRobot->setNumOfLives(this->numOfLives());
                                upgradedRobot->setNumOfKills(this->numOfKills());
                                upgradedRobot->setNumOfShells(this->numOfShell());
                                upgradedRobot->setUpgradeCount(this->getUpgradeCount());

                                int robotName_pos = this->robotName().find("_");
                                upgradedRobot->setRobotName(upgradedRobot->id() + this->robotName().substr(robotName_pos));

                                // Replace the robot in the battlefield vector immediately
                                for (auto& robot : battlefield->robots()) {
                                    if (robot == this) {
                                        robot = upgradedRobot;
                                        break; // important: break to avoid double replacement
                                    }
                                }

                                return;
                            }
                        }
                        case 2: {
                            // Randomly choose Shooting upgrade (1 - LongShotBot, 2 - SemiAutoBot, 3 - ThirtyShotBot)
                            int shootChoice = rand() % 3 + 1;
                            if (shootChoice == 1) {
                                cout <<this->id() <<" are now upgraded to LongShotBot!"<< endl;
                                outputLines.push_back(this->id() + " are now upgraded to LongShotBot!");
                                LongShotBot* upgradedRobot = new LongShotBot(this->id(), this->x(), this->y());

                                // Copy state
                                upgradedRobot->setNumOfLives(this->numOfLives());
                                upgradedRobot->setNumOfKills(this->numOfKills());
                                upgradedRobot->setNumOfShells(this->numOfShell());
                                upgradedRobot->setUpgradeCount(this->getUpgradeCount());

                                int robotName_pos = this->robotName().find("_");
                                upgradedRobot->setRobotName(upgradedRobot->id() + this->robotName().substr(robotName_pos));

                                // Replace the robot in the battlefield vector immediately
                                for (auto& robot : battlefield->robots()) {
                                    if (robot == this) {
                                        robot = upgradedRobot;
                                        break; // important: break to avoid double replacement
                                    }
                                }

                                return;
                            } else if (shootChoice == 2) {
                                cout <<this->id() <<" are now upgraded to SemiAutoBot!"<< endl;
                                outputLines.push_back(this->id() + " are now upgraded to SemiAutoBot!");
                                SemiAutoBot* upgradedRobot = new SemiAutoBot(this->id(), this->x(), this->y());

                                // Copy state
                                upgradedRobot->setNumOfLives(this->numOfLives());
                                upgradedRobot->setNumOfKills(this->numOfKills());
                                upgradedRobot->setNumOfShells(this->numOfShell());
                                upgradedRobot->setUpgradeCount(this->getUpgradeCount());

                                int robotName_pos = this->robotName().find("_");
                                upgradedRobot->setRobotName(upgradedRobot->id() + this->robotName().substr(robotName_pos));

                                // Replace the robot in the battlefield vector immediately
                                for (auto& robot : battlefield->robots()) {
                                    if (robot == this) {
                                        robot = upgradedRobot;
                                        break; // important: break to avoid double replacement
                                    }
                                }

                                return;
                            } else {
                                cout <<this->id() <<" are now upgraded to ThirtyShotBot!"<< endl;
                                outputLines.push_back(this->id() + " are now upgraded to ThirtyShotBot!");
                                ThirtyShotBot* upgradedRobot = new ThirtyShotBot(this->id(), this->x(), this->y());

                                // Copy state
                                upgradedRobot->setNumOfLives(this->numOfLives());
                                upgradedRobot->setNumOfKills(this->numOfKills());
                                // upgradedRobot->setNumOfShells(this->numOfShell());
                                upgradedRobot->setUpgradeCount(this->getUpgradeCount());

                                int robotName_pos = this->robotName().find("_");
                                upgradedRobot->setRobotName(upgradedRobot->id() + this->robotName().substr(robotName_pos));

                                // Replace the robot in the battlefield vector immediately
                                for (auto& robot : battlefield->robots()) {
                                    if (robot == this) {
                                        robot = upgradedRobot;
                                        break; // important: break to avoid double replacement
                                    }
                                }

                                return;
                            }
                            break;
                        }
                        // case 3: {
                        //     // Randomly choose Seeing upgrade (1 - ScoutBot, 2 - TrackBot)
                        //     int seeChoice = rand() % 2 + 1;
                        //     if (seeChoice == 1) {
                        //         cout <<this->id() <<" are now upgraded to ScoutBot!"<< endl;
                        //         outputLines.push_back(this->id() + " are now upgraded to ScoutBot!");
                        //         ScoutBot* upgradedRobot = new ScoutBot(this->id(), this->x(), this->y());

                        //         // Copy state
                        //         upgradedRobot->setNumOfLives(this->numOfLives());
                        //         upgradedRobot->setNumOfKills(this->numOfKills());
                        //         upgradedRobot->setNumOfShells(this->numOfShell());
                        //         upgradedRobot->setUpgradeCount(this->getUpgradeCount());

                        //         int robotName_pos = this->robotName().find("_");
                        //         upgradedRobot->setRobotName(upgradedRobot->id() + this->robotName().substr(robotName_pos));

                        //         // Replace the robot in the battlefield vector immediately
                        //         for (auto& robot : battlefield->robots()) {
                        //             if (robot == this) {
                        //                 robot = upgradedRobot;
                        //                 break; // important: break to avoid double replacement
                        //             }
                        //         }

                        //         return;
                        //     } else {
                        //         cout <<this->id() <<" are now upgraded to TrackBot!"<< endl;
                        //         outputLines.push_back(this->id() + " are now upgraded to TrackBot!");
                        //         TrackBot* upgradedRobot = new TrackBot(this->id(), this->x(), this->y());

                        //         // Copy state
                        //         upgradedRobot->setNumOfLives(this->numOfLives());
                        //         upgradedRobot->setNumOfKills(this->numOfKills());
                        //         upgradedRobot->setNumOfShells(this->numOfShell());
                        //         upgradedRobot->setUpgradeCount(this->getUpgradeCount());

                        //         int robotName_pos = this->robotName().find("_");
                        //         upgradedRobot->setRobotName(upgradedRobot->id() + this->robotName().substr(robotName_pos));

                        //         // Replace the robot in the battlefield vector immediately
                        //         for (auto& robot : battlefield->robots()) {
                        //             if (robot == this) {
                        //                 robot = upgradedRobot;
                        //                 break; // important: break to avoid double replacement
                        //             }
                        //         }

                        //         return;
                        //     }
                        //     break;
                        // }
                        case 3: {
                            // Randomly choose Extra upgrade (1 - HealthBot, 2 - GodBot, 3 - JukeBot)
                            int extraChoice = rand() % 3 + 1;
                            if (extraChoice == 1) {
                                cout << this->id() << " are now upgraded to HealthBot!" << endl;
                                outputLines.push_back(this->id() + " are now upgraded to HealthBot!");
                                HealthBot* upgradedRobot = new HealthBot(this->id(), this->x(), this->y());

                                // Copy state
                                upgradedRobot->setNumOfLives(this->numOfLives());
                                upgradedRobot->setNumOfKills(this->numOfKills());
                                upgradedRobot->setNumOfShells(this->numOfShell());
                                upgradedRobot->setUpgradeCount(this->getUpgradeCount());

                                int robotName_pos = this->robotName().find("_");
                                upgradedRobot->setRobotName(upgradedRobot->id() + this->robotName().substr(robotName_pos));

                                // Replace the robot in the battlefield vector immediately
                                for (auto& robot : battlefield->robots()) {
                                    if (robot == this) {
                                        robot = upgradedRobot;
                                        break; // important: break to avoid double replacement
                                    }
                                }

                                return;
                            } else if (extraChoice == 2) {
                                cout << this->id() << " are now upgraded to GodBot!" << endl;
                                outputLines.push_back(this->id() + " are now upgraded to GodBot!");
                                GodBot* upgradedRobot = new GodBot(this->id(), this->x(), this->y());

                                // Copy state
                                upgradedRobot->setNumOfLives(this->numOfLives());
                                upgradedRobot->setNumOfKills(this->numOfKills());
                                upgradedRobot->setNumOfShells(this->numOfShell());
                                upgradedRobot->setUpgradeCount(this->getUpgradeCount());

                                int robotName_pos = this->robotName().find("_");
                                upgradedRobot->setRobotName(upgradedRobot->id() + this->robotName().substr(robotName_pos));

                                // Replace the robot in the battlefield vector immediately
                                for (auto& robot : battlefield->robots()) {
                                    if (robot == this) {
                                        robot = upgradedRobot;
                                        break; // important: break to avoid double replacement
                                    }
                                }

                                return;
                            } else {
                                cout <<this->id() <<" are now upgraded to JukeBot!"<< endl;
                                outputLines.push_back(this->id() + " are now upgraded to JukeBot!");
                                JukeBot* upgradedRobot = new JukeBot(this->id(), this->x(), this->y());

                                // Copy state
                                upgradedRobot->setNumOfLives(this->numOfLives());
                                upgradedRobot->setNumOfKills(this->numOfKills());
                                upgradedRobot->setNumOfShells(this->numOfShell());
                                upgradedRobot->setUpgradeCount(this->getUpgradeCount());

                                int robotName_pos = this->robotName().find("_");
                                upgradedRobot->setRobotName(upgradedRobot->id() + this->robotName().substr(robotName_pos));

                                // Replace the robot in the battlefield vector immediately
                                for (auto& robot : battlefield->robots()) {
                                    if (robot == this) {
                                        robot = upgradedRobot;
                                        break; // important: break to avoid double replacement
                                    }
                                }

                                return;
                            }
                            break;
                        }
                    }

                } else {
                    cout << "Upgrade limit reached. Can only upgrade twice." << endl;
                    outputLines.push_back(this->id() + " are now upgraded to JukeBot!");
                }
            } else {
                this->decreaseShell();
                cout<<"Shot missed! The enemy robot was not hit."<<endl;
                cout<< this->id() <<" now has "<< this->numOfShell() <<" of shells left!"<<endl;

                outputLines.push_back("Shot missed! The enemy robot was not hit.");
                outputLines.push_back(this->id() + " now has " + to_string(this->numOfShell()) + " of shells left!");
            }
            hit = true;
        }
    }

    if (!hit) {
        this->decreaseShell();
        cout<<"No enemy robot was at the selected location."<<endl;
        cout<< this->id() <<" now has "<< this->numOfShell() <<" of shells left!"<<endl;

        outputLines.push_back("No enemy robot was at the selected location.");
        outputLines.push_back(this->id() + " now has " + to_string(this->numOfShell()) + " of shells left!");

    }
}


// LongSHotBot Action Logics
void LongShotBot::actionThink(Battlefield* battlefield) {
    cout << "LongShotBot is analyzing long-range targets..." << endl;
    outputLines.push_back("LongShotBot is analyzing long-range targets...");
}

void LongShotBot::actionLook(Battlefield* battlefield) {
    int currentX = robotPosX;
    int currentY = robotPosY;

    cout << "=== Robot View (3x3 Grid) ===" << endl;
    outputLines.push_back("=== Robot View (3x3 Grid) ===");

    for (int dy = -1; dy <= 1; ++dy) {
        string row = "";
        for (int dx = -1; dx <= 1; ++dx) {
            int x = currentX + dx;
            int y = currentY + dy;

            if (y >= 0 && y < battlefield->BATTLEFIELD_NUM_OF_ROWS() &&
                x >= 0 && x < battlefield->BATTLEFIELD_NUM_OF_COLS()) {

                if (x == currentX && y == currentY) {
                    cout << "[R] ";
                    row += "[R] ";
                } else {
                    string content = battlefield->getCellContent(x, y);
                    if (content.empty()) {
                        cout << "[ ] ";
                        row += "[ ] ";
                    } else {
                        cout << "[" << content.substr(0, 1) << "] ";
                        row += "[" + content.substr(0, 1) + "] ";
                    }
                }
            } else {
                cout << "[#] ";
                row += "[#] ";
            }
        }
        cout << endl;
        outputLines.push_back(row);
    }
}

void LongShotBot::actionMove(Battlefield* battlefield) {
    int currentX = robotPosX;
    int currentY = robotPosY;

    vector<string> directions = {"UL", "U", "UR", "L", "C", "R", "DL", "D", "DR"};
    vector<pair<int, int>> directionsMove = {
        {-1, -1}, {0, -1}, {1, -1},
        {-1, 0},  {0, 0},  {1, 0},
        {-1, 1},  {0, 1},  {1, 1}
    };

    cout << "\nAvailable Directions:\n";
    cout << "UL, U, UR\n";
    cout << "L, C, R\n";
    cout << "DL, D, DR\n";

    outputLines.push_back("\nAvailable Directions:");
    outputLines.push_back("UL, U, UR");
    outputLines.push_back("L, C, R");
    outputLines.push_back("DL, D, DR");

    // store valid index of move location  into validIndex
    vector<int> validIndex;
    for (int i = 0; i < directions.size(); ++i) {
        int newX = currentX + directionsMove[i].first;
        int newY = currentY + directionsMove[i].second;

        // Check bounds
        if (newX < 0 || newX >= battlefield->BATTLEFIELD_NUM_OF_COLS() ||
            newY < 0 || newY >= battlefield->BATTLEFIELD_NUM_OF_ROWS())
            continue;

        // Check if destination is occupied
        bool isOccupied = false;
        for (Robot* robot : battlefield->robots()) {
            if (robot->x() == newX && robot->y() == newY) {
                isOccupied = true;
                break;
            }
        }

        if (!isOccupied) {
            validIndex.push_back(i); // push back the index of the valid move
        }
    }

    // Handle case when no valid directions
    if (validIndex.empty()) {
        cout << "No valid move directions available\n";
        outputLines.push_back("No valid move directions available");
        return;
    }

    // Randomly pick a valid direction
    int chosenIndex = validIndex[rand() % validIndex.size()];
    string userInput = directions[chosenIndex];

    cout << "Random direction selected: " << userInput << endl;
    outputLines.push_back("Random direction selected: " + userInput);

    // Calculate new position
    int newX = currentX + directionsMove[chosenIndex].first;
    int newY = currentY + directionsMove[chosenIndex].second;

    // Update position
    robotPosX = newX;
    robotPosY = newY;
    battlefield->setCell(currentX, currentY, nullptr);
    battlefield->setCell(newX, newY, this);

    cout << "Moved to (" << newX << "," << newY << ")" << endl;
    outputLines.push_back("Moved to (" + to_string(newX) + "," + to_string(newY) + ")");
}

void LongShotBot::actionShoot(Battlefield* battlefield) {

    string CurrentRobotsName = this->robotName();
    cout << "The Current Robot controlled is " << CurrentRobotsName << endl;
    outputLines.push_back("The Current Robot controlled is " + CurrentRobotsName);

    int CurrentRobotsX = this->x();
    int CurrentRobotsY = this->y();

    int targetX, targetY;
    bool validTarget = false;

    int battlefieldWidth = battlefield->BATTLEFIELD_NUM_OF_COLS();
    int battlefieldHeight = battlefield->BATTLEFIELD_NUM_OF_ROWS();

    // Generate list of valid targets within Manhattan distance <= 3 (excluding self)
    vector<pair<int, int>> possibleTargets;
    for (int dx = -3; dx <= 3; dx++) {
        for (int dy = -3; dy <= 3; dy++) {
            if (dx == 0 && dy == 0) continue; // Skip self
            if (abs(dx) + abs(dy) > 3) continue; // Skip if Manhattan distance > 3

            int newX = CurrentRobotsX + dx;
            int newY = CurrentRobotsY + dy;

            // Check if within bounds
            if (newX >= 0 && newX < battlefieldWidth && newY >= 0 && newY < battlefieldHeight) {
                possibleTargets.emplace_back(newX, newY);
            }
        }
    }

    if (!possibleTargets.empty()) {
        int index = rand() % possibleTargets.size();
        targetX = possibleTargets[index].first;
        targetY = possibleTargets[index].second;

        cout << "Random target selected at: " << targetX << " " << targetY << endl;
        outputLines.push_back("Random target selected at: " + to_string(targetX) + " " + to_string(targetY));

        validTarget = true;
    } else {
        cout << "No valid tiles found for targeting." << endl;
        outputLines.push_back("No valid tiles found for targeting.");
        return;
    }

    // Shell check
    int ShellLeft = this->numOfShell();
    if (ShellLeft == 0) {
        cout << "Oh no! You ran out of shells! You can't shoot anything! YOU ARE SELF DESTRUCTING!" << endl;
        outputLines.push_back("Oh no! You ran out of shells! You can't shoot anything! YOU ARE SELF DESTRUCTING!");
        this->reduceLives();
        this->setNumOfShells(10);

        return;
    }

    bool hit = false;

    for (Robot* robot : battlefield->robots()) {
        string targetRobotId = robot->id() ;
        int PotentialRobotX = robot->x() ;
        int PotentialRobotY = robot->y() ;
        if (targetX == CurrentRobotsX && targetY == CurrentRobotsY ){
            cout<<"\nYou can't Shoot Yourself"<<endl;
            outputLines.push_back("\nYou can't Shoot Yourself");
            break;
        }
        if (targetX == PotentialRobotX && targetY == PotentialRobotY ){
            if (!robot->canBeTargeted()) {
                cout << "Target is hidden and cannot be hit!" << endl;
                outputLines.push_back("Target is hidden and cannot be hit!");
                return;
            }
            int hitChance = rand() % 100; // number from 0-99
            if (hitChance < 70) { // 70% chance to hit
                cout<<"\nYou've successfully shot an enemy Robot!"<<endl;
                outputLines.push_back("\nYou've successfully shot an enemy Robot!");
                robot->reduceLives();

                if (!robot->isAlive()) {
                    cout << targetRobotId << " has been destroyed!" << endl;
                    outputLines.push_back(targetRobotId + " has been destroyed!");

                    battlefield->setCell(PotentialRobotX, PotentialRobotY, nullptr); // remove robot from battlefield
                    battlefield->clearCell(PotentialRobotX, PotentialRobotY);
                    robot->setX(-1);
                    robot->setY(-2);

                    battlefield->destroyedRobots().push(robot);  // add to destroyed queue

                    // // Optional: If destroyed robots should be removed from active list
                    // auto& robotsVec = battlefield->robots();
                    // auto it = find(robotsVec.begin(), robotsVec.end(), robot);
                    // if (it != robotsVec.end()) {
                    //     robotsVec.erase(it);
                    // }

                } else {
                    cout << targetRobotId << " was hit and is now temporarily inactive!" << endl;
                    outputLines.push_back(targetRobotId + " was hit and is now temporarily inactive!");

                    battlefield->setCell(PotentialRobotX, PotentialRobotY, nullptr); // remove temporarily
                    battlefield->clearCell(PotentialRobotX, PotentialRobotY);
                    robot->setX(-1);
                    robot->setY(-1);

                    battlefield->waitingRobots().push({robot, 0}); // add to waiting queue

                    // auto& robotsVec = battlefield->robots();
                    // auto it = find(robotsVec.begin(), robotsVec.end(), robot);
                    // if (it != robotsVec.end()) {
                    //     robotsVec.erase(it);
                    // }
                }


                this->increaseKills();
                this->decreaseShell();
                int lifeLeft = robot->numOfLives();

                cout<< targetRobotId<<" now has "<<lifeLeft<<" of lives left"<<endl;
                cout<< this->id() <<" now has "<< this->numOfKills() <<" of kills!"<<endl;
                cout<< this->id() <<" now has "<< this->numOfShell() <<" of shells left!"<<endl;

                outputLines.push_back(targetRobotId + " now has " + to_string(lifeLeft) + " of lives left");
                outputLines.push_back(this->id() + " now has " + to_string(this->numOfKills()) + " of kills!");
                outputLines.push_back(this->id() + " now has " + to_string(this->numOfShell()) + " of shells left!");

                // cout << "Remaining Robots: " << endl;
                // for (const auto& r : battlefield->robots()) {
                //     cout << *r << endl;
                // }


                if (this->canUpgrade()) {
                    this->incrementUpgradeCount();

                    cout<<"\n" << this->id() <<" earned an upgrade! A random upgrade is applied!" << endl;
                    outputLines.push_back("\n" + this->id() + " earned an upgrade! A random upgrade is applied!");
                    //cout <<this->id() <<" are now upgraded to ScoutBot!"<< endl;
                    // ScoutBot* upgradedRobot = new ScoutBot(this->id().substr(5), this->x(), this->y());
                    // IM LOSING MY SHIT HERE

                    //Randomly choose upgrade category (1 - Moving, 2 - Seeing, 3 - Extras)
                    int choice = rand() % 3 + 1;

                    switch (choice) {
                        case 1: {
                            // Randomly choose Moving upgrade (1 - HideBot, 2 - JumpBot)
                            int moveChoice = rand() % 2 + 1;
                            if (moveChoice == 1) {
                                cout <<this->id() <<" are now upgraded to HideBot!"<< endl;
                                outputLines.push_back(this->id() + " are now upgraded to HideBot!");

                                HideBot* upgradedRobot = new HideBot(this->id(), this->x(), this->y());

                                // Copy state
                                upgradedRobot->setNumOfLives(this->numOfLives());
                                upgradedRobot->setNumOfKills(this->numOfKills());
                                upgradedRobot->setNumOfShells(this->numOfShell());
                                upgradedRobot->setUpgradeCount(this->getUpgradeCount());

                                int robotName_pos = this->robotName().find("_");
                                upgradedRobot->setRobotName(upgradedRobot->id() + this->robotName().substr(robotName_pos));

                                // Replace the robot in the battlefield vector immediately
                                for (auto& robot : battlefield->robots()) {
                                    if (robot == this) {
                                        robot = upgradedRobot;
                                        break; // important: break to avoid double replacement
                                    }
                                }

                                return;
                            } else {
                                cout <<this->id() <<" are now upgraded to JumpBot!"<< endl;
                                outputLines.push_back(this->id() + " are now upgraded to JumpBot!");
                                JumpBot* upgradedRobot = new JumpBot(this->id(), this->x(), this->y());

                                // Copy state
                                upgradedRobot->setNumOfLives(this->numOfLives());
                                upgradedRobot->setNumOfKills(this->numOfKills());
                                upgradedRobot->setNumOfShells(this->numOfShell());
                                upgradedRobot->setUpgradeCount(this->getUpgradeCount());

                                int robotName_pos = this->robotName().find("_");
                                upgradedRobot->setRobotName(upgradedRobot->id() + this->robotName().substr(robotName_pos));

                                // Replace the robot in the battlefield vector immediately
                                for (auto& robot : battlefield->robots()) {
                                    if (robot == this) {
                                        robot = upgradedRobot;
                                        break; // important: break to avoid double replacement
                                    }
                                }

                                return;
                            }
                        }
                        // case 2: {
                        //     // Randomly choose Shooting upgrade (1 - LongShotBot, 2 - SemiAutoBot, 3 - ThirtyShotBot)
                        //     int shootChoice = rand() % 3 + 1;
                        //     if (shootChoice == 1) {
                        //         cout <<this->id() <<" are now upgraded to LongShotBot!"<< endl;
                        //         outputLines.push_back(this->id() + " are now upgraded to LongShotBot!");
                        //         LongShotBot* upgradedRobot = new LongShotBot(this->id(), this->x(), this->y());

                        //         // Copy state
                        //         upgradedRobot->setNumOfLives(this->numOfLives());
                        //         upgradedRobot->setNumOfKills(this->numOfKills());
                        //         upgradedRobot->setNumOfShells(this->numOfShell());
                        //         upgradedRobot->setUpgradeCount(this->getUpgradeCount());

                        //         int robotName_pos = this->robotName().find("_");
                        //         upgradedRobot->setRobotName(upgradedRobot->id() + this->robotName().substr(robotName_pos));

                        //         // Replace the robot in the battlefield vector immediately
                        //         for (auto& robot : battlefield->robots()) {
                        //             if (robot == this) {
                        //                 robot = upgradedRobot;
                        //                 break; // important: break to avoid double replacement
                        //             }
                        //         }

                        //         return;
                        //     } else if (shootChoice == 2) {
                        //         cout <<this->id() <<" are now upgraded to SemiAutoBot!"<< endl;
                        //         outputLines.push_back(this->id() + " are now upgraded to SemiAutoBot!");
                        //         SemiAutoBot* upgradedRobot = new SemiAutoBot(this->id(), this->x(), this->y());

                        //         // Copy state
                        //         upgradedRobot->setNumOfLives(this->numOfLives());
                        //         upgradedRobot->setNumOfKills(this->numOfKills());
                        //         upgradedRobot->setNumOfShells(this->numOfShell());
                        //         upgradedRobot->setUpgradeCount(this->getUpgradeCount());

                        //         int robotName_pos = this->robotName().find("_");
                        //         upgradedRobot->setRobotName(upgradedRobot->id() + this->robotName().substr(robotName_pos));

                        //         // Replace the robot in the battlefield vector immediately
                        //         for (auto& robot : battlefield->robots()) {
                        //             if (robot == this) {
                        //                 robot = upgradedRobot;
                        //                 break; // important: break to avoid double replacement
                        //             }
                        //         }

                        //         return;
                        //     } else {
                        //         cout <<this->id() <<" are now upgraded to ThirtyShotBot!"<< endl;
                        //         outputLines.push_back(this->id() + " are now upgraded to ThirtyShotBot!");
                        //         ThirtyShotBot* upgradedRobot = new ThirtyShotBot(this->id(), this->x(), this->y());

                        //         // Copy state
                        //         upgradedRobot->setNumOfLives(this->numOfLives());
                        //         upgradedRobot->setNumOfKills(this->numOfKills());
                        //         upgradedRobot->setNumOfShells(this->numOfShell());
                        //         upgradedRobot->setUpgradeCount(this->getUpgradeCount());

                        //         int robotName_pos = this->robotName().find("_");
                        //         upgradedRobot->setRobotName(upgradedRobot->id() + this->robotName().substr(robotName_pos));

                        //         // Replace the robot in the battlefield vector immediately
                        //         for (auto& robot : battlefield->robots()) {
                        //             if (robot == this) {
                        //                 robot = upgradedRobot;
                        //                 break; // important: break to avoid double replacement
                        //             }
                        //         }

                        //         return;
                        //     }
                        //     break;
                        // }
                        case 2: {
                            // Randomly choose Seeing upgrade (1 - ScoutBot, 2 - TrackBot)
                            int seeChoice = rand() % 2 + 1;
                            if (seeChoice == 1) {
                                cout <<this->id() <<" are now upgraded to ScoutBot!"<< endl;
                                outputLines.push_back(this->id() + " are now upgraded to ScoutBot!");
                                ScoutBot* upgradedRobot = new ScoutBot(this->id(), this->x(), this->y());

                                // Copy state
                                upgradedRobot->setNumOfLives(this->numOfLives());
                                upgradedRobot->setNumOfKills(this->numOfKills());
                                upgradedRobot->setNumOfShells(this->numOfShell());
                                upgradedRobot->setUpgradeCount(this->getUpgradeCount());

                                int robotName_pos = this->robotName().find("_");
                                upgradedRobot->setRobotName(upgradedRobot->id() + this->robotName().substr(robotName_pos));

                                // Replace the robot in the battlefield vector immediately
                                for (auto& robot : battlefield->robots()) {
                                    if (robot == this) {
                                        robot = upgradedRobot;
                                        break; // important: break to avoid double replacement
                                    }
                                }

                                return;
                            } else {
                                cout <<this->id() <<" are now upgraded to TrackBot!"<< endl;
                                outputLines.push_back(this->id() + " are now upgraded to TrackBot!");
                                TrackBot* upgradedRobot = new TrackBot(this->id(), this->x(), this->y());

                                // Copy state
                                upgradedRobot->setNumOfLives(this->numOfLives());
                                upgradedRobot->setNumOfKills(this->numOfKills());
                                upgradedRobot->setNumOfShells(this->numOfShell());
                                upgradedRobot->setUpgradeCount(this->getUpgradeCount());

                                int robotName_pos = this->robotName().find("_");
                                upgradedRobot->setRobotName(upgradedRobot->id() + this->robotName().substr(robotName_pos));

                                // Replace the robot in the battlefield vector immediately
                                for (auto& robot : battlefield->robots()) {
                                    if (robot == this) {
                                        robot = upgradedRobot;
                                        break; // important: break to avoid double replacement
                                    }
                                }

                                return;
                            }
                            break;
                        }
                        case 3: {
                            // Randomly choose Extra upgrade (1 - HealthBot, 2 - GodBot, 3 - JukeBot)
                            int extraChoice = rand() % 3 + 1;
                            if (extraChoice == 1) {
                                cout << this->id() << " are now upgraded to HealthBot!" << endl;
                                outputLines.push_back(this->id() + " are now upgraded to HealthBot!");
                                HealthBot* upgradedRobot = new HealthBot(this->id(), this->x(), this->y());

                                // Copy state
                                upgradedRobot->setNumOfLives(this->numOfLives());
                                upgradedRobot->setNumOfKills(this->numOfKills());
                                upgradedRobot->setNumOfShells(this->numOfShell());
                                upgradedRobot->setUpgradeCount(this->getUpgradeCount());

                                int robotName_pos = this->robotName().find("_");
                                upgradedRobot->setRobotName(upgradedRobot->id() + this->robotName().substr(robotName_pos));

                                // Replace the robot in the battlefield vector immediately
                                for (auto& robot : battlefield->robots()) {
                                    if (robot == this) {
                                        robot = upgradedRobot;
                                        break; // important: break to avoid double replacement
                                    }
                                }

                                return;
                            } else if (extraChoice == 2) {
                                cout << this->id() << " are now upgraded to GodBot!" << endl;
                                outputLines.push_back(this->id() + " are now upgraded to GodBot!");
                                GodBot* upgradedRobot = new GodBot(this->id(), this->x(), this->y());

                                // Copy state
                                upgradedRobot->setNumOfLives(this->numOfLives());
                                upgradedRobot->setNumOfKills(this->numOfKills());
                                upgradedRobot->setNumOfShells(this->numOfShell());
                                upgradedRobot->setUpgradeCount(this->getUpgradeCount());

                                int robotName_pos = this->robotName().find("_");
                                upgradedRobot->setRobotName(upgradedRobot->id() + this->robotName().substr(robotName_pos));

                                // Replace the robot in the battlefield vector immediately
                                for (auto& robot : battlefield->robots()) {
                                    if (robot == this) {
                                        robot = upgradedRobot;
                                        break; // important: break to avoid double replacement
                                    }
                                }

                                return;
                            } else {
                                cout <<this->id() <<" are now upgraded to JukeBot!"<< endl;
                                outputLines.push_back(this->id() + " are now upgraded to JukeBot!");
                                JukeBot* upgradedRobot = new JukeBot(this->id(), this->x(), this->y());

                                // Copy state
                                upgradedRobot->setNumOfLives(this->numOfLives());
                                upgradedRobot->setNumOfKills(this->numOfKills());
                                upgradedRobot->setNumOfShells(this->numOfShell());
                                upgradedRobot->setUpgradeCount(this->getUpgradeCount());

                                int robotName_pos = this->robotName().find("_");
                                upgradedRobot->setRobotName(upgradedRobot->id() + this->robotName().substr(robotName_pos));

                                // Replace the robot in the battlefield vector immediately
                                for (auto& robot : battlefield->robots()) {
                                    if (robot == this) {
                                        robot = upgradedRobot;
                                        break; // important: break to avoid double replacement
                                    }
                                }

                                return;
                            }
                            break;
                        }
                    }

                } else {
                    cout << "Upgrade limit reached. Can only upgrade twice." << endl;
                    outputLines.push_back(this->id() + " are now upgraded to JukeBot!");
                }
            } else {
                this->decreaseShell();
                cout<<"Shot missed! The enemy robot was not hit."<<endl;
                cout<< this->id() <<" now has "<< this->numOfShell() <<" of shells left!"<<endl;

                outputLines.push_back("Shot missed! The enemy robot was not hit.");
                outputLines.push_back(this->id() + " now has " + to_string(this->numOfShell()) + " of shells left!");
            }
            hit = true;
        }
    }

    if (!hit) {
        this->decreaseShell();
        cout<<"No enemy robot was at the selected location."<<endl;
        cout<< this->id() <<" now has "<< this->numOfShell() <<" of shells left!"<<endl;

        outputLines.push_back("No enemy robot was at the selected location.");
        outputLines.push_back(this->id() + " now has " + to_string(this->numOfShell()) + " of shells left!");

    }
}


// SemiAutoBot Action Logics
void SemiAutoBot::actionThink(Battlefield* battlefield) {
    cout << "SemiAutoBot is analyzing long-range targets..." << endl;
    outputLines.push_back("SemiAutoBot is analyzing long-range targets...");
}

void SemiAutoBot::actionLook(Battlefield* battlefield) {
    int currentX = robotPosX;
    int currentY = robotPosY;

    cout << "=== Robot View (3x3 Grid) ===" << endl;
    outputLines.push_back("=== Robot View (3x3 Grid) ===");

    for (int dy = -1; dy <= 1; ++dy) {
        string row = "";
        for (int dx = -1; dx <= 1; ++dx) {
            int x = currentX + dx;
            int y = currentY + dy;

            if (y >= 0 && y < battlefield->BATTLEFIELD_NUM_OF_ROWS() &&
                x >= 0 && x < battlefield->BATTLEFIELD_NUM_OF_COLS()) {

                if (x == currentX && y == currentY) {
                    cout << "[R] ";
                    row += "[R] ";
                } else {
                    string content = battlefield->getCellContent(x, y);
                    if (content.empty()) {
                        cout << "[ ] ";
                        row += "[ ] ";
                    } else {
                        cout << "[" << content.substr(0, 1) << "] ";
                        row += "[" + content.substr(0, 1) + "] ";
                    }
                }
            } else {
                cout << "[#] ";
                row += "[#] ";
            }
        }
        cout << endl;
        outputLines.push_back(row);
    }
}

void SemiAutoBot::actionMove(Battlefield* battlefield) {
    int currentX = robotPosX;
    int currentY = robotPosY;

    vector<string> directions = {"UL", "U", "UR", "L", "C", "R", "DL", "D", "DR"};
    vector<pair<int, int>> directionsMove = {
        {-1, -1}, {0, -1}, {1, -1},
        {-1, 0},  {0, 0},  {1, 0},
        {-1, 1},  {0, 1},  {1, 1}
    };

    cout << "\nAvailable Directions:\n";
    cout << "UL, U, UR\n";
    cout << "L, C, R\n";
    cout << "DL, D, DR\n";

    outputLines.push_back("\nAvailable Directions:");
    outputLines.push_back("UL, U, UR");
    outputLines.push_back("L, C, R");
    outputLines.push_back("DL, D, DR");

    // store valid index of move location  into validIndex
    vector<int> validIndex;
    for (int i = 0; i < directions.size(); ++i) {
        int newX = currentX + directionsMove[i].first;
        int newY = currentY + directionsMove[i].second;

        // Check bounds
        if (newX < 0 || newX >= battlefield->BATTLEFIELD_NUM_OF_COLS() ||
            newY < 0 || newY >= battlefield->BATTLEFIELD_NUM_OF_ROWS())
            continue;

        // Check if destination is occupied
        bool isOccupied = false;
        for (Robot* robot : battlefield->robots()) {
            if (robot->x() == newX && robot->y() == newY) {
                isOccupied = true;
                break;
            }
        }

        if (!isOccupied) {
            validIndex.push_back(i); // push back the index of the valid move
        }
    }

    // Handle case when no valid directions
    if (validIndex.empty()) {
        cout << "No valid move directions available\n";
        outputLines.push_back("No valid move directions available");
        return;
    }

    // Randomly pick a valid direction
    int chosenIndex = validIndex[rand() % validIndex.size()];
    string userInput = directions[chosenIndex];

    cout << "Random direction selected: " << userInput << endl;
    outputLines.push_back("Random direction selected: " + userInput);

    // Calculate new position
    int newX = currentX + directionsMove[chosenIndex].first;
    int newY = currentY + directionsMove[chosenIndex].second;

    // Update position
    robotPosX = newX;
    robotPosY = newY;
    battlefield->setCell(currentX, currentY, nullptr);
    battlefield->setCell(newX, newY, this);

    cout << "Moved to (" << newX << "," << newY << ")" << endl;
    outputLines.push_back("Moved to (" + to_string(newX) + "," + to_string(newY) + ")");
}

void SemiAutoBot::actionShoot(Battlefield* battlefield) {

    string CurrentRobotsName = this->robotName();
    cout << "The Current Robot controlled is " << CurrentRobotsName << endl;
    outputLines.push_back("The Current Robot controlled is " + CurrentRobotsName);

    int CurrentRobotsX = this->x();
    int CurrentRobotsY = this->y();

    int targetX, targetY;
    bool validTarget = false;

    int battlefieldWidth = battlefield->BATTLEFIELD_NUM_OF_COLS();
    int battlefieldHeight = battlefield->BATTLEFIELD_NUM_OF_ROWS();

    // Generate list of valid adjacent targets
    vector<pair<int, int>> possibleTargets;
    for (int dx = -1; dx <= 1; dx++) {
        for (int dy = -1; dy <= 1; dy++) {
            if (dx == 0 && dy == 0) continue; // Skip self

            int newX = CurrentRobotsX + dx;
            int newY = CurrentRobotsY + dy;

            // Check if within bounds
            if (newX >= 0 && newX < battlefieldWidth && newY >= 0 && newY < battlefieldHeight) {
                possibleTargets.emplace_back(newX, newY);
            }
        }
    }

    if (!possibleTargets.empty()) {
        int index = rand() % possibleTargets.size();
        targetX = possibleTargets[index].first;
        targetY = possibleTargets[index].second;

        cout << "Random target selected at: " << targetX << " " << targetY << endl;
        outputLines.push_back("Random target selected at: " + to_string(targetX) + " " + to_string(targetY));

        validTarget = true;
    } else {
        cout << "No valid adjacent tiles found for targeting." << endl;
        outputLines.push_back("No valid adjacent tiles found for targeting.");
        return;
    }

    // Shell check
    int ShellLeft = this->numOfShell();
    if (ShellLeft == 0) {
        cout << "Oh no! You ran out of shells! You can't shoot anything! YOU ARE SELF DESTRUCTING!" << endl;
        outputLines.push_back("Oh no! You ran out of shells! You can't shoot anything! YOU ARE SELF DESTRUCTING!");
        this->reduceLives();
        this->setNumOfShells(10);

        return;
    }

    bool hit = false;

    for (Robot* robot : battlefield->robots()) {
        string targetRobotId = robot->id() ;
        int PotentialRobotX = robot->x() ;
        int PotentialRobotY = robot->y() ;
        if (targetX == CurrentRobotsX && targetY == CurrentRobotsY ){
            cout<<"\nYou can't Shoot Yourself"<<endl;
            outputLines.push_back("\nYou can't Shoot Yourself");
            break;
        }
        if (targetX == PotentialRobotX && targetY == PotentialRobotY ){
            if (!robot->canBeTargeted()) {
                cout << "Target is hidden and cannot be hit!" << endl;
                outputLines.push_back("Target is hidden and cannot be hit!");
                return;
            }
            bool threeShotSuccess = false;
            int hitChance;
            for (int i = 0; i < 3 ; i++){
                int chance = rand() % 100; // number from 0-99
                if (chance < 70){
                    threeShotSuccess = true;
                    break;
                }
            }
            if (threeShotSuccess = true){
                hitChance = 69;
            } else if (threeShotSuccess = false) {
                hitChance = 80;
            }


            if (hitChance < 70) { // 70% chance to hit
                cout<<"\nYou've successfully shot an enemy Robot!"<<endl;
                outputLines.push_back("\nYou've successfully shot an enemy Robot!");
                robot->reduceLives();

                if (!robot->isAlive()) {
                    cout << targetRobotId << " has been destroyed!" << endl;
                    outputLines.push_back(targetRobotId + " has been destroyed!");

                    battlefield->setCell(PotentialRobotX, PotentialRobotY, nullptr); // remove robot from battlefield
                    battlefield->clearCell(PotentialRobotX, PotentialRobotY);
                    robot->setX(-1);
                    robot->setY(-2);

                    battlefield->destroyedRobots().push(robot);  // add to destroyed queue

                    // // Optional: If destroyed robots should be removed from active list
                    // auto& robotsVec = battlefield->robots();
                    // auto it = find(robotsVec.begin(), robotsVec.end(), robot);
                    // if (it != robotsVec.end()) {
                    //     robotsVec.erase(it);
                    // }

                } else {
                    cout << targetRobotId << " was hit and is now temporarily inactive!" << endl;
                    outputLines.push_back(targetRobotId + " was hit and is now temporarily inactive!");

                    battlefield->setCell(PotentialRobotX, PotentialRobotY, nullptr); // remove temporarily
                    battlefield->clearCell(PotentialRobotX, PotentialRobotY);
                    robot->setX(-1);
                    robot->setY(-1);

                    battlefield->waitingRobots().push({robot, 0}); // add to waiting queue

                    // auto& robotsVec = battlefield->robots();
                    // auto it = find(robotsVec.begin(), robotsVec.end(), robot);
                    // if (it != robotsVec.end()) {
                    //     robotsVec.erase(it);
                    // }
                }


                this->increaseKills();
                this->decreaseShell();
                int lifeLeft = robot->numOfLives();

                cout<< targetRobotId<<" now has "<<lifeLeft<<" of lives left"<<endl;
                cout<< this->id() <<" now has "<< this->numOfKills() <<" of kills!"<<endl;
                cout<< this->id() <<" now has "<< this->numOfShell() <<" of shells left!"<<endl;

                outputLines.push_back(targetRobotId + " now has " + to_string(lifeLeft) + " of lives left");
                outputLines.push_back(this->id() + " now has " + to_string(this->numOfKills()) + " of kills!");
                outputLines.push_back(this->id() + " now has " + to_string(this->numOfShell()) + " of shells left!");

                // cout << "Remaining Robots: " << endl;
                // for (const auto& r : battlefield->robots()) {
                //     cout << *r << endl;
                // }


                if (this->canUpgrade()) {
                    this->incrementUpgradeCount();

                    cout<<"\n" << this->id() <<" earned an upgrade! A random upgrade is applied!" << endl;
                    outputLines.push_back("\n" + this->id() + " earned an upgrade! A random upgrade is applied!");
                    //cout <<this->id() <<" are now upgraded to ScoutBot!"<< endl;
                    // ScoutBot* upgradedRobot = new ScoutBot(this->id().substr(5), this->x(), this->y());
                    // IM LOSING MY SHIT HERE

                    //Randomly choose upgrade category (1 - Moving, 2 - Shooting, 3 - Extras)
                    int choice = rand() % 3 + 1;

                    switch (choice) {
                        case 1: {
                            // Randomly choose Moving upgrade (1 - HideBot, 2 - JumpBot)
                            int moveChoice = rand() % 2 + 1;
                            if (moveChoice == 1) {
                                cout <<this->id() <<" are now upgraded to HideBot!"<< endl;
                                outputLines.push_back(this->id() + " are now upgraded to HideBot!");

                                HideBot* upgradedRobot = new HideBot(this->id(), this->x(), this->y());

                                // Copy state
                                upgradedRobot->setNumOfLives(this->numOfLives());
                                upgradedRobot->setNumOfKills(this->numOfKills());
                                upgradedRobot->setNumOfShells(this->numOfShell());
                                upgradedRobot->setUpgradeCount(this->getUpgradeCount());

                                int robotName_pos = this->robotName().find("_");
                                upgradedRobot->setRobotName(upgradedRobot->id() + this->robotName().substr(robotName_pos));

                                // Replace the robot in the battlefield vector immediately
                                for (auto& robot : battlefield->robots()) {
                                    if (robot == this) {
                                        robot = upgradedRobot;
                                        break; // important: break to avoid double replacement
                                    }
                                }

                                return;
                            } else {
                                cout <<this->id() <<" are now upgraded to JumpBot!"<< endl;
                                outputLines.push_back(this->id() + " are now upgraded to JumpBot!");
                                JumpBot* upgradedRobot = new JumpBot(this->id(), this->x(), this->y());

                                // Copy state
                                upgradedRobot->setNumOfLives(this->numOfLives());
                                upgradedRobot->setNumOfKills(this->numOfKills());
                                upgradedRobot->setNumOfShells(this->numOfShell());
                                upgradedRobot->setUpgradeCount(this->getUpgradeCount());

                                int robotName_pos = this->robotName().find("_");
                                upgradedRobot->setRobotName(upgradedRobot->id() + this->robotName().substr(robotName_pos));

                                // Replace the robot in the battlefield vector immediately
                                for (auto& robot : battlefield->robots()) {
                                    if (robot == this) {
                                        robot = upgradedRobot;
                                        break; // important: break to avoid double replacement
                                    }
                                }

                                return;
                            }
                        }
                        // case 2: {
                        //     // Randomly choose Shooting upgrade (1 - LongShotBot, 2 - SemiAutoBot, 3 - ThirtyShotBot)
                        //     int shootChoice = rand() % 3 + 1;
                        //     if (shootChoice == 1) {
                        //         cout <<this->id() <<" are now upgraded to LongShotBot!"<< endl;
                        //         outputLines.push_back(this->id() + " are now upgraded to LongShotBot!");
                        //         LongShotBot* upgradedRobot = new LongShotBot(this->id(), this->x(), this->y());

                        //         // Copy state
                        //         upgradedRobot->setNumOfLives(this->numOfLives());
                        //         upgradedRobot->setNumOfKills(this->numOfKills());
                        //         upgradedRobot->setNumOfShells(this->numOfShell());
                        //         upgradedRobot->setUpgradeCount(this->getUpgradeCount());

                        //         int robotName_pos = this->robotName().find("_");
                        //         upgradedRobot->setRobotName(upgradedRobot->id() + this->robotName().substr(robotName_pos));

                        //         // Replace the robot in the battlefield vector immediately
                        //         for (auto& robot : battlefield->robots()) {
                        //             if (robot == this) {
                        //                 robot = upgradedRobot;
                        //                 break; // important: break to avoid double replacement
                        //             }
                        //         }

                        //         return;
                        //     } else if (shootChoice == 2) {
                        //         cout <<this->id() <<" are now upgraded to SemiAutoBot!"<< endl;
                        //         outputLines.push_back(this->id() + " are now upgraded to SemiAutoBot!");
                        //         SemiAutoBot* upgradedRobot = new SemiAutoBot(this->id(), this->x(), this->y());

                        //         // Copy state
                        //         upgradedRobot->setNumOfLives(this->numOfLives());
                        //         upgradedRobot->setNumOfKills(this->numOfKills());
                        //         upgradedRobot->setNumOfShells(this->numOfShell());
                        //         upgradedRobot->setUpgradeCount(this->getUpgradeCount());

                        //         int robotName_pos = this->robotName().find("_");
                        //         upgradedRobot->setRobotName(upgradedRobot->id() + this->robotName().substr(robotName_pos));

                        //         // Replace the robot in the battlefield vector immediately
                        //         for (auto& robot : battlefield->robots()) {
                        //             if (robot == this) {
                        //                 robot = upgradedRobot;
                        //                 break; // important: break to avoid double replacement
                        //             }
                        //         }

                        //         return;
                        //     } else {
                        //         cout <<this->id() <<" are now upgraded to ThirtyShotBot!"<< endl;
                        //         outputLines.push_back(this->id() + " are now upgraded to ThirtyShotBot!");
                        //         ThirtyShotBot* upgradedRobot = new ThirtyShotBot(this->id(), this->x(), this->y());

                        //         // Copy state
                        //         upgradedRobot->setNumOfLives(this->numOfLives());
                        //         upgradedRobot->setNumOfKills(this->numOfKills());
                        //         upgradedRobot->setNumOfShells(this->numOfShell());
                        //         upgradedRobot->setUpgradeCount(this->getUpgradeCount());

                        //         int robotName_pos = this->robotName().find("_");
                        //         upgradedRobot->setRobotName(upgradedRobot->id() + this->robotName().substr(robotName_pos));

                        //         // Replace the robot in the battlefield vector immediately
                        //         for (auto& robot : battlefield->robots()) {
                        //             if (robot == this) {
                        //                 robot = upgradedRobot;
                        //                 break; // important: break to avoid double replacement
                        //             }
                        //         }

                        //         return;
                        //     }
                        //     break;
                        // }
                        case 2: {
                            // Randomly choose Seeing upgrade (1 - ScoutBot, 2 - TrackBot)
                            int seeChoice = rand() % 2 + 1;
                            if (seeChoice == 1) {
                                cout <<this->id() <<" are now upgraded to ScoutBot!"<< endl;
                                outputLines.push_back(this->id() + " are now upgraded to ScoutBot!");
                                ScoutBot* upgradedRobot = new ScoutBot(this->id(), this->x(), this->y());

                                // Copy state
                                upgradedRobot->setNumOfLives(this->numOfLives());
                                upgradedRobot->setNumOfKills(this->numOfKills());
                                upgradedRobot->setNumOfShells(this->numOfShell());
                                upgradedRobot->setUpgradeCount(this->getUpgradeCount());

                                int robotName_pos = this->robotName().find("_");
                                upgradedRobot->setRobotName(upgradedRobot->id() + this->robotName().substr(robotName_pos));

                                // Replace the robot in the battlefield vector immediately
                                for (auto& robot : battlefield->robots()) {
                                    if (robot == this) {
                                        robot = upgradedRobot;
                                        break; // important: break to avoid double replacement
                                    }
                                }

                                return;
                            } else {
                                cout <<this->id() <<" are now upgraded to TrackBot!"<< endl;
                                outputLines.push_back(this->id() + " are now upgraded to TrackBot!");
                                TrackBot* upgradedRobot = new TrackBot(this->id(), this->x(), this->y());

                                // Copy state
                                upgradedRobot->setNumOfLives(this->numOfLives());
                                upgradedRobot->setNumOfKills(this->numOfKills());
                                upgradedRobot->setNumOfShells(this->numOfShell());
                                upgradedRobot->setUpgradeCount(this->getUpgradeCount());

                                int robotName_pos = this->robotName().find("_");
                                upgradedRobot->setRobotName(upgradedRobot->id() + this->robotName().substr(robotName_pos));

                                // Replace the robot in the battlefield vector immediately
                                for (auto& robot : battlefield->robots()) {
                                    if (robot == this) {
                                        robot = upgradedRobot;
                                        break; // important: break to avoid double replacement
                                    }
                                }

                                return;
                            }
                            break;
                        }
                        case 3: {
                            // Randomly choose Extra upgrade (1 - HealthBot, 2 - GodBot, 3 - JukeBot)
                            int extraChoice = rand() % 3 + 1;
                            if (extraChoice == 1) {
                                cout << this->id() << " are now upgraded to HealthBot!" << endl;
                                outputLines.push_back(this->id() + " are now upgraded to HealthBot!");
                                HealthBot* upgradedRobot = new HealthBot(this->id(), this->x(), this->y());

                                // Copy state
                                upgradedRobot->setNumOfLives(this->numOfLives());
                                upgradedRobot->setNumOfKills(this->numOfKills());
                                upgradedRobot->setNumOfShells(this->numOfShell());
                                upgradedRobot->setUpgradeCount(this->getUpgradeCount());

                                int robotName_pos = this->robotName().find("_");
                                upgradedRobot->setRobotName(upgradedRobot->id() + this->robotName().substr(robotName_pos));

                                // Replace the robot in the battlefield vector immediately
                                for (auto& robot : battlefield->robots()) {
                                    if (robot == this) {
                                        robot = upgradedRobot;
                                        break; // important: break to avoid double replacement
                                    }
                                }

                                return;
                            } else if (extraChoice == 2) {
                                cout << this->id() << " are now upgraded to GodBot!" << endl;
                                outputLines.push_back(this->id() + " are now upgraded to GodBot!");
                                GodBot* upgradedRobot = new GodBot(this->id(), this->x(), this->y());

                                // Copy state
                                upgradedRobot->setNumOfLives(this->numOfLives());
                                upgradedRobot->setNumOfKills(this->numOfKills());
                                upgradedRobot->setNumOfShells(this->numOfShell());
                                upgradedRobot->setUpgradeCount(this->getUpgradeCount());

                                int robotName_pos = this->robotName().find("_");
                                upgradedRobot->setRobotName(upgradedRobot->id() + this->robotName().substr(robotName_pos));

                                // Replace the robot in the battlefield vector immediately
                                for (auto& robot : battlefield->robots()) {
                                    if (robot == this) {
                                        robot = upgradedRobot;
                                        break; // important: break to avoid double replacement
                                    }
                                }

                                return;
                            } else {
                                cout <<this->id() <<" are now upgraded to JukeBot!"<< endl;
                                outputLines.push_back(this->id() + " are now upgraded to JukeBot!");
                                JukeBot* upgradedRobot = new JukeBot(this->id(), this->x(), this->y());

                                // Copy state
                                upgradedRobot->setNumOfLives(this->numOfLives());
                                upgradedRobot->setNumOfKills(this->numOfKills());
                                upgradedRobot->setNumOfShells(this->numOfShell());
                                upgradedRobot->setUpgradeCount(this->getUpgradeCount());

                                int robotName_pos = this->robotName().find("_");
                                upgradedRobot->setRobotName(upgradedRobot->id() + this->robotName().substr(robotName_pos));

                                // Replace the robot in the battlefield vector immediately
                                for (auto& robot : battlefield->robots()) {
                                    if (robot == this) {
                                        robot = upgradedRobot;
                                        break; // important: break to avoid double replacement
                                    }
                                }

                                return;
                            }
                            break;
                        }
                    }

                } else {
                    cout << "Upgrade limit reached. Can only upgrade twice." << endl;
                    outputLines.push_back(this->id() + " are now upgraded to JukeBot!");
                }
            } else {
                this->decreaseShell();
                cout<<"Shot missed! The enemy robot was not hit."<<endl;
                cout<< this->id() <<" now has "<< this->numOfShell() <<" of shells left!"<<endl;

                outputLines.push_back("Shot missed! The enemy robot was not hit.");
                outputLines.push_back(this->id() + " now has " + to_string(this->numOfShell()) + " of shells left!");
            }
            hit = true;
        }
    }

    if (!hit) {
        this->decreaseShell();
        cout<<"No enemy robot was at the selected location."<<endl;
        cout<< this->id() <<" now has "<< this->numOfShell() <<" of shells left!"<<endl;

        outputLines.push_back("No enemy robot was at the selected location.");
        outputLines.push_back(this->id() + " now has " + to_string(this->numOfShell()) + " of shells left!");

    }
}


// ThirtyShotBot Action Logics
void ThirtyShotBot::actionThink(Battlefield* battlefield) {
    cout << "ThirtyShotBot is analyzing targets..." << endl;
    outputLines.push_back("ThirtyShotBot is analyzing targets...");

}

void ThirtyShotBot::actionLook(Battlefield* battlefield) {
    int currentX = robotPosX;
    int currentY = robotPosY;

    cout << "=== Robot View (3x3 Grid) ===" << endl;
    outputLines.push_back("=== Robot View (3x3 Grid) ===");

    for (int dy = -1; dy <= 1; ++dy) {
        string row = "";
        for (int dx = -1; dx <= 1; ++dx) {
            int x = currentX + dx;
            int y = currentY + dy;

            if (y >= 0 && y < battlefield->BATTLEFIELD_NUM_OF_ROWS() &&
                x >= 0 && x < battlefield->BATTLEFIELD_NUM_OF_COLS()) {

                if (x == currentX && y == currentY) {
                    cout << "[R] ";
                    row += "[R] ";
                } else {
                    string content = battlefield->getCellContent(x, y);
                    if (content.empty()) {
                        cout << "[ ] ";
                        row += "[ ] ";
                    } else {
                        cout << "[" << content.substr(0, 1) << "] ";
                        row += "[" + content.substr(0, 1) + "] ";
                    }
                }
            } else {
                cout << "[#] ";
                row += "[#] ";
            }
        }
        cout << endl;
        outputLines.push_back(row);
    }
}

void ThirtyShotBot::actionMove(Battlefield* battlefield) {
    int currentX = robotPosX;
    int currentY = robotPosY;

    vector<string> directions = {"UL", "U", "UR", "L", "C", "R", "DL", "D", "DR"};
    vector<pair<int, int>> directionsMove = {
        {-1, -1}, {0, -1}, {1, -1},
        {-1, 0},  {0, 0},  {1, 0},
        {-1, 1},  {0, 1},  {1, 1}
    };

    cout << "\nAvailable Directions:\n";
    cout << "UL, U, UR\n";
    cout << "L, C, R\n";
    cout << "DL, D, DR\n";

    outputLines.push_back("\nAvailable Directions:");
    outputLines.push_back("UL, U, UR");
    outputLines.push_back("L, C, R");
    outputLines.push_back("DL, D, DR");

    // store valid index of move location  into validIndex
    vector<int> validIndex;
    for (int i = 0; i < directions.size(); ++i) {
        int newX = currentX + directionsMove[i].first;
        int newY = currentY + directionsMove[i].second;

        // Check bounds
        if (newX < 0 || newX >= battlefield->BATTLEFIELD_NUM_OF_COLS() ||
            newY < 0 || newY >= battlefield->BATTLEFIELD_NUM_OF_ROWS())
            continue;

        // Check if destination is occupied
        bool isOccupied = false;
        for (Robot* robot : battlefield->robots()) {
            if (robot->x() == newX && robot->y() == newY) {
                isOccupied = true;
                break;
            }
        }

        if (!isOccupied) {
            validIndex.push_back(i); // push back the index of the valid move
        }
    }

    // Handle case when no valid directions
    if (validIndex.empty()) {
        cout << "No valid move directions available\n";
        outputLines.push_back("No valid move directions available");
        return;
    }

    // Randomly pick a valid direction
    int chosenIndex = validIndex[rand() % validIndex.size()];
    string userInput = directions[chosenIndex];

    cout << "Random direction selected: " << userInput << endl;
    outputLines.push_back("Random direction selected: " + userInput);

    // Calculate new position
    int newX = currentX + directionsMove[chosenIndex].first;
    int newY = currentY + directionsMove[chosenIndex].second;

    // Update position
    robotPosX = newX;
    robotPosY = newY;
    battlefield->setCell(currentX, currentY, nullptr);
    battlefield->setCell(newX, newY, this);

    cout << "Moved to (" << newX << "," << newY << ")" << endl;
    outputLines.push_back("Moved to (" + to_string(newX) + "," + to_string(newY) + ")");
}

void ThirtyShotBot::actionShoot(Battlefield* battlefield) {

    string CurrentRobotsName = this->robotName();
    cout << "The Current Robot controlled is " << CurrentRobotsName << endl;
    outputLines.push_back("The Current Robot controlled is " + CurrentRobotsName);

    int CurrentRobotsX = this->x();
    int CurrentRobotsY = this->y();

    int targetX, targetY;
    bool validTarget = false;

    int battlefieldWidth = battlefield->BATTLEFIELD_NUM_OF_COLS();
    int battlefieldHeight = battlefield->BATTLEFIELD_NUM_OF_ROWS();

    // Generate list of valid adjacent targets
    vector<pair<int, int>> possibleTargets;
    for (int dx = -1; dx <= 1; dx++) {
        for (int dy = -1; dy <= 1; dy++) {
            if (dx == 0 && dy == 0) continue; // Skip self

            int newX = CurrentRobotsX + dx;
            int newY = CurrentRobotsY + dy;

            // Check if within bounds
            if (newX >= 0 && newX < battlefieldWidth && newY >= 0 && newY < battlefieldHeight) {
                possibleTargets.emplace_back(newX, newY);
            }
        }
    }

    if (!possibleTargets.empty()) {
        int index = rand() % possibleTargets.size();
        targetX = possibleTargets[index].first;
        targetY = possibleTargets[index].second;

        cout << "Random target selected at: " << targetX << " " << targetY << endl;
        outputLines.push_back("Random target selected at: " + to_string(targetX) + " " + to_string(targetY));

        validTarget = true;
    } else {
        cout << "No valid adjacent tiles found for targeting." << endl;
        outputLines.push_back("No valid adjacent tiles found for targeting.");
        return;
    }

    // Shell check
    int ShellLeft = this->numOfShell();
    if (ShellLeft == 0) {
        cout << "Oh no! You ran out of shells! You can't shoot anything! YOU ARE SELF DESTRUCTING!" << endl;
        outputLines.push_back("Oh no! You ran out of shells! You can't shoot anything! YOU ARE SELF DESTRUCTING!");
        this->reduceLives();
        this->setNumOfShells(30);
        return;
    }

    bool hit = false;

    for (Robot* robot : battlefield->robots()) {
        string targetRobotId = robot->id() ;
        int PotentialRobotX = robot->x() ;
        int PotentialRobotY = robot->y() ;
        if (targetX == CurrentRobotsX && targetY == CurrentRobotsY ){
            cout<<"\nYou can't Shoot Yourself"<<endl;
            outputLines.push_back("\nYou can't Shoot Yourself");
            break;
        }
        if (targetX == PotentialRobotX && targetY == PotentialRobotY ){
            if (!robot->canBeTargeted()) {
                cout << "Target is hidden and cannot be hit!" << endl;
                outputLines.push_back("Target is hidden and cannot be hit!");
                return;
            }
            int hitChance = rand() % 100; // number from 0-99
            if (hitChance < 70) { // 70% chance to hit
                cout<<"\nYou've successfully shot an enemy Robot!"<<endl;
                outputLines.push_back("\nYou've successfully shot an enemy Robot!");
                robot->reduceLives();

                if (!robot->isAlive()) {
                    cout << targetRobotId << " has been destroyed!" << endl;
                    outputLines.push_back(targetRobotId + " has been destroyed!");

                    battlefield->setCell(PotentialRobotX, PotentialRobotY, nullptr); // remove robot from battlefield
                    battlefield->clearCell(PotentialRobotX, PotentialRobotY);
                    robot->setX(-1);
                    robot->setY(-2);

                    battlefield->destroyedRobots().push(robot);  // add to destroyed queue

                    // // Optional: If destroyed robots should be removed from active list
                    // auto& robotsVec = battlefield->robots();
                    // auto it = find(robotsVec.begin(), robotsVec.end(), robot);
                    // if (it != robotsVec.end()) {
                    //     robotsVec.erase(it);
                    // }

                } else {
                    cout << targetRobotId << " was hit and is now temporarily inactive!" << endl;
                    outputLines.push_back(targetRobotId + " was hit and is now temporarily inactive!");

                    battlefield->setCell(PotentialRobotX, PotentialRobotY, nullptr); // remove temporarily
                    battlefield->clearCell(PotentialRobotX, PotentialRobotY);
                    robot->setX(-1);
                    robot->setY(-1);

                    battlefield->waitingRobots().push({robot, 0}); // add to waiting queue

                    // auto& robotsVec = battlefield->robots();
                    // auto it = find(robotsVec.begin(), robotsVec.end(), robot);
                    // if (it != robotsVec.end()) {
                    //     robotsVec.erase(it);
                    // }
                }


                this->increaseKills();
                this->decreaseShell();
                int lifeLeft = robot->numOfLives();

                cout<< targetRobotId<<" now has "<<lifeLeft<<" of lives left"<<endl;
                cout<< this->id() <<" now has "<< this->numOfKills() <<" of kills!"<<endl;
                cout<< this->id() <<" now has "<< this->numOfShell() <<" of shells left!"<<endl;

                outputLines.push_back(targetRobotId + " now has " + to_string(lifeLeft) + " of lives left");
                outputLines.push_back(this->id() + " now has " + to_string(this->numOfKills()) + " of kills!");
                outputLines.push_back(this->id() + " now has " + to_string(this->numOfShell()) + " of shells left!");

                // cout << "Remaining Robots: " << endl;
                // for (const auto& r : battlefield->robots()) {
                //     cout << *r << endl;
                // }


                if (this->canUpgrade()) {
                    this->incrementUpgradeCount();

                    cout<<"\n" << this->id() <<" earned an upgrade! A random upgrade is applied!" << endl;
                    outputLines.push_back("\n" + this->id() + " earned an upgrade! A random upgrade is applied!");
                    //cout <<this->id() <<" are now upgraded to ScoutBot!"<< endl;
                    // ScoutBot* upgradedRobot = new ScoutBot(this->id().substr(5), this->x(), this->y());
                    // IM LOSING MY SHIT HERE

                    //Randomly choose upgrade category (1 - Moving, 2 - Shooting, 3 - Extras)
                    int choice = rand() % 3 + 1;

                    switch (choice) {
                        case 1: {
                            // Randomly choose Moving upgrade (1 - HideBot, 2 - JumpBot)
                            int moveChoice = rand() % 2 + 1;
                            if (moveChoice == 1) {
                                cout <<this->id() <<" are now upgraded to HideBot!"<< endl;
                                outputLines.push_back(this->id() + " are now upgraded to HideBot!");

                                HideBot* upgradedRobot = new HideBot(this->id(), this->x(), this->y());

                                // Copy state
                                upgradedRobot->setNumOfLives(this->numOfLives());
                                upgradedRobot->setNumOfKills(this->numOfKills());
                                upgradedRobot->setNumOfShells(this->numOfShell());
                                upgradedRobot->setUpgradeCount(this->getUpgradeCount());

                                int robotName_pos = this->robotName().find("_");
                                upgradedRobot->setRobotName(upgradedRobot->id() + this->robotName().substr(robotName_pos));

                                // Replace the robot in the battlefield vector immediately
                                for (auto& robot : battlefield->robots()) {
                                    if (robot == this) {
                                        robot = upgradedRobot;
                                        break; // important: break to avoid double replacement
                                    }
                                }

                                return;
                            } else {
                                cout <<this->id() <<" are now upgraded to JumpBot!"<< endl;
                                outputLines.push_back(this->id() + " are now upgraded to JumpBot!");
                                JumpBot* upgradedRobot = new JumpBot(this->id(), this->x(), this->y());

                                // Copy state
                                upgradedRobot->setNumOfLives(this->numOfLives());
                                upgradedRobot->setNumOfKills(this->numOfKills());
                                upgradedRobot->setNumOfShells(this->numOfShell());
                                upgradedRobot->setUpgradeCount(this->getUpgradeCount());

                                int robotName_pos = this->robotName().find("_");
                                upgradedRobot->setRobotName(upgradedRobot->id() + this->robotName().substr(robotName_pos));

                                // Replace the robot in the battlefield vector immediately
                                for (auto& robot : battlefield->robots()) {
                                    if (robot == this) {
                                        robot = upgradedRobot;
                                        break; // important: break to avoid double replacement
                                    }
                                }

                                return;
                            }
                        }
                        // case 2: {
                        //     // Randomly choose Shooting upgrade (1 - LongShotBot, 2 - SemiAutoBot, 3 - ThirtyShotBot)
                        //     int shootChoice = rand() % 3 + 1;
                        //     if (shootChoice == 1) {
                        //         cout <<this->id() <<" are now upgraded to LongShotBot!"<< endl;
                        //         outputLines.push_back(this->id() + " are now upgraded to LongShotBot!");
                        //         LongShotBot* upgradedRobot = new LongShotBot(this->id(), this->x(), this->y());

                        //         // Copy state
                        //         upgradedRobot->setNumOfLives(this->numOfLives());
                        //         upgradedRobot->setNumOfKills(this->numOfKills());
                        //         upgradedRobot->setNumOfShells(this->numOfShell());
                        //         upgradedRobot->setUpgradeCount(this->getUpgradeCount());

                        //         int robotName_pos = this->robotName().find("_");
                        //         upgradedRobot->setRobotName(upgradedRobot->id() + this->robotName().substr(robotName_pos));

                        //         // Replace the robot in the battlefield vector immediately
                        //         for (auto& robot : battlefield->robots()) {
                        //             if (robot == this) {
                        //                 robot = upgradedRobot;
                        //                 break; // important: break to avoid double replacement
                        //             }
                        //         }

                        //         return;
                        //     } else if (shootChoice == 2) {
                        //         cout <<this->id() <<" are now upgraded to SemiAutoBot!"<< endl;
                        //         outputLines.push_back(this->id() + " are now upgraded to SemiAutoBot!");
                        //         SemiAutoBot* upgradedRobot = new SemiAutoBot(this->id(), this->x(), this->y());

                        //         // Copy state
                        //         upgradedRobot->setNumOfLives(this->numOfLives());
                        //         upgradedRobot->setNumOfKills(this->numOfKills());
                        //         upgradedRobot->setNumOfShells(this->numOfShell());
                        //         upgradedRobot->setUpgradeCount(this->getUpgradeCount());

                        //         int robotName_pos = this->robotName().find("_");
                        //         upgradedRobot->setRobotName(upgradedRobot->id() + this->robotName().substr(robotName_pos));

                        //         // Replace the robot in the battlefield vector immediately
                        //         for (auto& robot : battlefield->robots()) {
                        //             if (robot == this) {
                        //                 robot = upgradedRobot;
                        //                 break; // important: break to avoid double replacement
                        //             }
                        //         }

                        //         return;
                        //     } else {
                        //         cout <<this->id() <<" are now upgraded to ThirtyShotBot!"<< endl;
                        //         outputLines.push_back(this->id() + " are now upgraded to ThirtyShotBot!");
                        //         ThirtyShotBot* upgradedRobot = new ThirtyShotBot(this->id(), this->x(), this->y());

                        //         // Copy state
                        //         upgradedRobot->setNumOfLives(this->numOfLives());
                        //         upgradedRobot->setNumOfKills(this->numOfKills());
                        //         upgradedRobot->setNumOfShells(this->numOfShell());
                        //         upgradedRobot->setUpgradeCount(this->getUpgradeCount());

                        //         int robotName_pos = this->robotName().find("_");
                        //         upgradedRobot->setRobotName(upgradedRobot->id() + this->robotName().substr(robotName_pos));

                        //         // Replace the robot in the battlefield vector immediately
                        //         for (auto& robot : battlefield->robots()) {
                        //             if (robot == this) {
                        //                 robot = upgradedRobot;
                        //                 break; // important: break to avoid double replacement
                        //             }
                        //         }

                        //         return;
                        //     }
                        //     break;
                        // }
                        case 2: {
                            // Randomly choose Seeing upgrade (1 - ScoutBot, 2 - TrackBot)
                            int seeChoice = rand() % 2 + 1;
                            if (seeChoice == 1) {
                                cout <<this->id() <<" are now upgraded to ScoutBot!"<< endl;
                                outputLines.push_back(this->id() + " are now upgraded to ScoutBot!");
                                ScoutBot* upgradedRobot = new ScoutBot(this->id(), this->x(), this->y());

                                // Copy state
                                upgradedRobot->setNumOfLives(this->numOfLives());
                                upgradedRobot->setNumOfKills(this->numOfKills());
                                upgradedRobot->setNumOfShells(this->numOfShell());
                                upgradedRobot->setUpgradeCount(this->getUpgradeCount());

                                int robotName_pos = this->robotName().find("_");
                                upgradedRobot->setRobotName(upgradedRobot->id() + this->robotName().substr(robotName_pos));

                                // Replace the robot in the battlefield vector immediately
                                for (auto& robot : battlefield->robots()) {
                                    if (robot == this) {
                                        robot = upgradedRobot;
                                        break; // important: break to avoid double replacement
                                    }
                                }

                                return;
                            } else {
                                cout <<this->id() <<" are now upgraded to TrackBot!"<< endl;
                                outputLines.push_back(this->id() + " are now upgraded to TrackBot!");
                                TrackBot* upgradedRobot = new TrackBot(this->id(), this->x(), this->y());

                                // Copy state
                                upgradedRobot->setNumOfLives(this->numOfLives());
                                upgradedRobot->setNumOfKills(this->numOfKills());
                                upgradedRobot->setNumOfShells(this->numOfShell());
                                upgradedRobot->setUpgradeCount(this->getUpgradeCount());

                                int robotName_pos = this->robotName().find("_");
                                upgradedRobot->setRobotName(upgradedRobot->id() + this->robotName().substr(robotName_pos));

                                // Replace the robot in the battlefield vector immediately
                                for (auto& robot : battlefield->robots()) {
                                    if (robot == this) {
                                        robot = upgradedRobot;
                                        break; // important: break to avoid double replacement
                                    }
                                }

                                return;
                            }
                            break;
                        }
                        case 3: {
                            // Randomly choose Extra upgrade (1 - HealthBot, 2 - GodBot, 3 - JukeBot)
                            int extraChoice = rand() % 3 + 1;
                            if (extraChoice == 1) {
                                cout << this->id() << " are now upgraded to HealthBot!" << endl;
                                outputLines.push_back(this->id() + " are now upgraded to HealthBot!");
                                HealthBot* upgradedRobot = new HealthBot(this->id(), this->x(), this->y());

                                // Copy state
                                upgradedRobot->setNumOfLives(this->numOfLives());
                                upgradedRobot->setNumOfKills(this->numOfKills());
                                upgradedRobot->setNumOfShells(this->numOfShell());
                                upgradedRobot->setUpgradeCount(this->getUpgradeCount());

                                int robotName_pos = this->robotName().find("_");
                                upgradedRobot->setRobotName(upgradedRobot->id() + this->robotName().substr(robotName_pos));

                                // Replace the robot in the battlefield vector immediately
                                for (auto& robot : battlefield->robots()) {
                                    if (robot == this) {
                                        robot = upgradedRobot;
                                        break; // important: break to avoid double replacement
                                    }
                                }

                                return;
                            } else if (extraChoice == 2) {
                                cout << this->id() << " are now upgraded to GodBot!" << endl;
                                outputLines.push_back(this->id() + " are now upgraded to GodBot!");
                                GodBot* upgradedRobot = new GodBot(this->id(), this->x(), this->y());

                                // Copy state
                                upgradedRobot->setNumOfLives(this->numOfLives());
                                upgradedRobot->setNumOfKills(this->numOfKills());
                                upgradedRobot->setNumOfShells(this->numOfShell());
                                upgradedRobot->setUpgradeCount(this->getUpgradeCount());

                                int robotName_pos = this->robotName().find("_");
                                upgradedRobot->setRobotName(upgradedRobot->id() + this->robotName().substr(robotName_pos));

                                // Replace the robot in the battlefield vector immediately
                                for (auto& robot : battlefield->robots()) {
                                    if (robot == this) {
                                        robot = upgradedRobot;
                                        break; // important: break to avoid double replacement
                                    }
                                }

                                return;
                            } else {
                                cout <<this->id() <<" are now upgraded to JukeBot!"<< endl;
                                outputLines.push_back(this->id() + " are now upgraded to JukeBot!");
                                JukeBot* upgradedRobot = new JukeBot(this->id(), this->x(), this->y());

                                // Copy state
                                upgradedRobot->setNumOfLives(this->numOfLives());
                                upgradedRobot->setNumOfKills(this->numOfKills());
                                upgradedRobot->setNumOfShells(this->numOfShell());
                                upgradedRobot->setUpgradeCount(this->getUpgradeCount());

                                int robotName_pos = this->robotName().find("_");
                                upgradedRobot->setRobotName(upgradedRobot->id() + this->robotName().substr(robotName_pos));

                                // Replace the robot in the battlefield vector immediately
                                for (auto& robot : battlefield->robots()) {
                                    if (robot == this) {
                                        robot = upgradedRobot;
                                        break; // important: break to avoid double replacement
                                    }
                                }

                                return;
                            }
                            break;
                        }
                    }

                } else {
                    cout << "Upgrade limit reached. Can only upgrade twice." << endl;
                    outputLines.push_back(this->id() + " are now upgraded to JukeBot!");
                }
            } else {
                this->decreaseShell();
                cout<<"Shot missed! The enemy robot was not hit."<<endl;
                cout<< this->id() <<" now has "<< this->numOfShell() <<" of shells left!"<<endl;

                outputLines.push_back("Shot missed! The enemy robot was not hit.");
                outputLines.push_back(this->id() + " now has " + to_string(this->numOfShell()) + " of shells left!");
            }
            hit = true;
        }
    }

    if (!hit) {
        this->decreaseShell();
        cout<<"No enemy robot was at the selected location."<<endl;
        cout<< this->id() <<" now has "<< this->numOfShell() <<" of shells left!"<<endl;

        outputLines.push_back("No enemy robot was at the selected location.");
        outputLines.push_back(this->id() + " now has " + to_string(this->numOfShell()) + " of shells left!");

    }
}


// HideBot Action Logics
void HideBot::actionThink(Battlefield* battlefield) {
    cout << "HideBot is thinking about hiding..." << endl;
    outputLines.push_back("HideBot is thinking about hiding...");

}

void HideBot::actionLook(Battlefield* battlefield) {
    int currentX = robotPosX;
    int currentY = robotPosY;

    cout << "=== Robot View (3x3 Grid) ===" << endl;
    outputLines.push_back("=== Robot View (3x3 Grid) ===");

    for (int dy = -1; dy <= 1; ++dy) {
        string row = "";
        for (int dx = -1; dx <= 1; ++dx) {
            int x = currentX + dx;
            int y = currentY + dy;

            if (y >= 0 && y < battlefield->BATTLEFIELD_NUM_OF_ROWS() &&
                x >= 0 && x < battlefield->BATTLEFIELD_NUM_OF_COLS()) {

                if (x == currentX && y == currentY) {
                    cout << "[R] ";
                    row += "[R] ";
                } else {
                    string content = battlefield->getCellContent(x, y);
                    if (content.empty()) {
                        cout << "[ ] ";
                        row += "[ ] ";
                    } else {
                        cout << "[" << content.substr(0, 1) << "] ";
                        row += "[" + content.substr(0, 1) + "] ";
                    }
                }
            } else {
                cout << "[#] ";
                row += "[#] ";
            }
        }
        cout << endl;
        outputLines.push_back(row);
    }
}

void HideBot::actionMove(Battlefield* battlefield) {
    int currentX = robotPosX;
    int currentY = robotPosY;

    vector<string> directions = {"UL", "U", "UR", "L", "C", "R", "DL", "D", "DR"};
    vector<pair<int, int>> directionsMove = {
        {-1, -1}, {0, -1}, {1, -1},
        {-1, 0},  {0, 0},  {1, 0},
        {-1, 1},  {0, 1},  {1, 1}
    };

    cout << "\nAvailable Directions:\n";
    cout << "UL, U, UR\n";
    cout << "L, C, R\n";
    cout << "DL, D, DR\n";

    outputLines.push_back("\nAvailable Directions:");
    outputLines.push_back("UL, U, UR");
    outputLines.push_back("L, C, R");
    outputLines.push_back("DL, D, DR");

    // store valid index of move location  into validIndex
    vector<int> validIndex;
    for (int i = 0; i < directions.size(); ++i) {
        int newX = currentX + directionsMove[i].first;
        int newY = currentY + directionsMove[i].second;

        // Check bounds
        if (newX < 0 || newX >= battlefield->BATTLEFIELD_NUM_OF_COLS() ||
            newY < 0 || newY >= battlefield->BATTLEFIELD_NUM_OF_ROWS())
            continue;

        // Check if destination is occupied
        bool isOccupied = false;
        for (Robot* robot : battlefield->robots()) {
            if (robot->x() == newX && robot->y() == newY) {
                isOccupied = true;
                break;
            }
        }

        if (!isOccupied) {
            validIndex.push_back(i); // push back the index of the valid move
        }
    }

    // Handle case when no valid directions
    if (validIndex.empty()) {
        cout << "No valid move directions available\n";
        outputLines.push_back("No valid move directions available");
        return;
    }

    // Randomly pick a valid direction
    int chosenIndex = validIndex[rand() % validIndex.size()];
    string userInput = directions[chosenIndex];

    cout << "Random direction selected: " << userInput << endl;
    outputLines.push_back("Random direction selected: " + userInput);

    // Calculate new position
    int newX = currentX + directionsMove[chosenIndex].first;
    int newY = currentY + directionsMove[chosenIndex].second;

    // Update position
    robotPosX = newX;
    robotPosY = newY;
    battlefield->setCell(currentX, currentY, nullptr);
    battlefield->setCell(newX, newY, this);

    cout << "Moved to (" << newX << "," << newY << ")" << endl;
    outputLines.push_back("Moved to (" + to_string(newX) + "," + to_string(newY) + ")");
}

void HideBot::actionShoot(Battlefield* battlefield) {

    string CurrentRobotsName = this->robotName();
    cout << "The Current Robot controlled is " << CurrentRobotsName << endl;
    outputLines.push_back("The Current Robot controlled is " + CurrentRobotsName);

    int CurrentRobotsX = this->x();
    int CurrentRobotsY = this->y();

    int targetX, targetY;
    bool validTarget = false;

    int battlefieldWidth = battlefield->BATTLEFIELD_NUM_OF_COLS();
    int battlefieldHeight = battlefield->BATTLEFIELD_NUM_OF_ROWS();

    vector<pair<int, int>> possibleTargets;
    for (int dx = -1; dx <= 1; dx++) {
        for (int dy = -1; dy <= 1; dy++) {
            if (dx == 0 && dy == 0) continue; // Skip self

            int newX = CurrentRobotsX + dx;
            int newY = CurrentRobotsY + dy;

            if (newX >= 0 && newX < battlefieldWidth && newY >= 0 && newY < battlefieldHeight) {
                possibleTargets.emplace_back(newX, newY);
            }
        }
    }

    if (!possibleTargets.empty()) {
        int index = rand() % possibleTargets.size();
        targetX = possibleTargets[index].first;
        targetY = possibleTargets[index].second;

        cout << "Random target selected at: " << targetX << " " << targetY << endl;
        outputLines.push_back("Random target selected at: " + to_string(targetX) + " " + to_string(targetY));

        validTarget = true;
    } else {
        cout << "No valid adjacent tiles found for targeting." << endl;
        outputLines.push_back("No valid adjacent tiles found for targeting.");
        return;
    }

    // Shell check
    int ShellLeft = this->numOfShell();
    if (ShellLeft == 0) {
        cout << "Oh no! You ran out of shells! You can't shoot anything! YOU ARE SELF DESTRUCTING!" << endl;
        outputLines.push_back("Oh no! You ran out of shells! You can't shoot anything! YOU ARE SELF DESTRUCTING!");
        this->reduceLives();
        this->setNumOfShells(10);
        return;
    }

    bool hit = false;


    for (Robot* robot : battlefield->robots()) {
        string targetRobotId = robot->id() ;
        int PotentialRobotX = robot->x() ;
        int PotentialRobotY = robot->y() ;
        if (targetX == CurrentRobotsX && targetY == CurrentRobotsY ){
            cout<<"\nYou can't Shoot Yourself"<<endl;
            outputLines.push_back("\nYou can't Shoot Yourself");

            break;
        }
        if (targetX == PotentialRobotX && targetY == PotentialRobotY ){
            if (!robot->canBeTargeted()) {
                cout << "Target is hidden and cannot be hit!" << endl;
                outputLines.push_back("Target is hidden and cannot be hit!");
                return;
            }
            int hitChance = rand() % 100; // number from 0-99
            if (hitChance < 70) { // 70% chance to hit
                cout<<"\nYou've successfully shot an enemy Robot!"<<endl;
                outputLines.push_back("\nYou've successfully shot an enemy Robot!");

                robot->reduceLives();

                if (!robot->isAlive()) {
                    cout << targetRobotId << " has been destroyed!" << endl;
                outputLines.push_back(targetRobotId + " has been destroyed!" );


                    battlefield->setCell(PotentialRobotX, PotentialRobotY, nullptr); // remove robot from battlefield
                    battlefield->clearCell(PotentialRobotX, PotentialRobotY);
                    robot->setX(-1);
                    robot->setY(-2);

                    battlefield->destroyedRobots().push(robot);  // add to destroyed queue

                    // // Optional: If destroyed robots should be removed from active list
                    // auto& robotsVec = battlefield->robots();
                    // auto it = find(robotsVec.begin(), robotsVec.end(), robot);
                    // if (it != robotsVec.end()) {
                    //     robotsVec.erase(it);
                    // }

                } else {
                    cout << targetRobotId << " was hit and is now temporarily inactive!" << endl;
                    outputLines.push_back( targetRobotId + " was hit and is now temporarily inactive!"  );


                    battlefield->setCell(PotentialRobotX, PotentialRobotY, nullptr); // remove temporarily
                    battlefield->clearCell(PotentialRobotX, PotentialRobotY);
                    robot->setX(-1);
                    robot->setY(-1);

                    battlefield->waitingRobots().push({robot, 0}); // add to waiting queue

                    // auto& robotsVec = battlefield->robots();
                    // auto it = find(robotsVec.begin(), robotsVec.end(), robot);
                    // if (it != robotsVec.end()) {
                    //     robotsVec.erase(it);
                    // }
                }


                this->increaseKills();
                this->decreaseShell();
                int lifeLeft = robot->numOfLives();
                cout<< targetRobotId<<" now has "<<lifeLeft<<" of lives left"<<endl;
                outputLines.push_back(targetRobotId + " now has " + to_string(lifeLeft)  + " of lives left");

                cout<< this->id() <<" now has "<< this->numOfKills() <<" of kills!"<<endl;
                outputLines.push_back(this->id() + " now has " + to_string(this->numOfKills()) + " of kills!");

                cout<< this->id() <<" now has "<< this->numOfShell() <<" of shells left!"<<endl;
                outputLines.push_back(this->id() + " now has " + to_string(this->numOfShell()) + " of shells left!");


                // cout << "Remaining Robots: " << endl;
                // for (const auto& r : battlefield->robots()) {
                //     cout << *r << endl;
                // }


                if (this->canUpgrade()) {
                    this->incrementUpgradeCount();

                    cout<<"\n" << this->id() <<" earned an upgrade! A random upgrade is applied!" << endl;
                    outputLines.push_back("\n" + this->id() + " earned an upgrade! A random upgrade is applied!");

                    //cout <<this->id() <<" are now upgraded to ScoutBot!"<< endl;
                // ScoutBot* upgradedRobot = new ScoutBot(this->id().substr(5), this->x(), this->y());
                    // IM LOSING MY SHIT HERE

                    //Randomly choose upgrade category (1 - Moving, 2 - Shooting, 3 - Seeing, 4 - Extras)
                    int choice = rand() % 3 + 1;

                    switch (choice) {
                        // case 1: {
                        //     // Randomly choose Moving upgrade (1 - HideBot, 2 - JumpBot)
                        //     int moveChoice = rand() % 2 + 1;
                        //     if (moveChoice == 1) {
                        //         cout <<this->id() <<" are now upgraded to HideBot!"<< endl;
                        //         outputLines.push_back(this->id() + " are now upgraded to HideBot!");

                        //         HideBot* upgradedRobot = new HideBot(this->id(), this->x(), this->y());

                        //         // Copy state
                        //         upgradedRobot->setNumOfLives(this->numOfLives());
                        //         upgradedRobot->setNumOfKills(this->numOfKills());
                        //         upgradedRobot->setNumOfShells(this->numOfShell());
                        //         upgradedRobot->setUpgradeCount(this->getUpgradeCount());

                        //         int robotName_pos = this->robotName().find("_");
                        //         upgradedRobot->setRobotName(upgradedRobot->id() + this->robotName().substr(robotName_pos));

                        //         // Replace the robot in the battlefield vector immediately
                        //         for (auto& robot : battlefield->robots()) {
                        //             if (robot == this) {
                        //                 robot = upgradedRobot;
                        //                 break; // important: break to avoid double replacement
                        //             }
                        //         }

                        //         return;
                        //     } else {
                        //         cout <<this->id() <<" are now upgraded to JumpBot!"<< endl;
                        //         outputLines.push_back(this->id() + " are now upgraded to JumpBot!");
                        //         JumpBot* upgradedRobot = new JumpBot(this->id(), this->x(), this->y());

                        //         // Copy state
                        //         upgradedRobot->setNumOfLives(this->numOfLives());
                        //         upgradedRobot->setNumOfKills(this->numOfKills());
                        //         upgradedRobot->setNumOfShells(this->numOfShell());
                        //         upgradedRobot->setUpgradeCount(this->getUpgradeCount());

                        //         int robotName_pos = this->robotName().find("_");
                        //         upgradedRobot->setRobotName(upgradedRobot->id() + this->robotName().substr(robotName_pos));

                        //         // Replace the robot in the battlefield vector immediately
                        //         for (auto& robot : battlefield->robots()) {
                        //             if (robot == this) {
                        //                 robot = upgradedRobot;
                        //                 break; // important: break to avoid double replacement
                        //             }
                        //         }

                        //         return;
                        //     }
                        // }
                        case 1: {
                            // Randomly choose Shooting upgrade (1 - LongShotBot, 2 - SemiAutoBot, 3 - ThirtyShotBot)
                            int shootChoice = rand() % 3 + 1;
                            if (shootChoice == 1) {
                                cout <<this->id() <<" are now upgraded to LongShotBot!"<< endl;
                                outputLines.push_back(this->id() + " are now upgraded to LongShotBot!");
                                LongShotBot* upgradedRobot = new LongShotBot(this->id(), this->x(), this->y());

                                // Copy state
                                upgradedRobot->setNumOfLives(this->numOfLives());
                                upgradedRobot->setNumOfKills(this->numOfKills());
                                upgradedRobot->setNumOfShells(this->numOfShell());
                                upgradedRobot->setUpgradeCount(this->getUpgradeCount());

                                int robotName_pos = this->robotName().find("_");
                                upgradedRobot->setRobotName(upgradedRobot->id() + this->robotName().substr(robotName_pos));

                                // Replace the robot in the battlefield vector immediately
                                for (auto& robot : battlefield->robots()) {
                                    if (robot == this) {
                                        robot = upgradedRobot;
                                        break; // important: break to avoid double replacement
                                    }
                                }

                                return;
                            } else if (shootChoice == 2) {
                                cout <<this->id() <<" are now upgraded to SemiAutoBot!"<< endl;
                                outputLines.push_back(this->id() + " are now upgraded to SemiAutoBot!");
                                SemiAutoBot* upgradedRobot = new SemiAutoBot(this->id(), this->x(), this->y());

                                // Copy state
                                upgradedRobot->setNumOfLives(this->numOfLives());
                                upgradedRobot->setNumOfKills(this->numOfKills());
                                upgradedRobot->setNumOfShells(this->numOfShell());
                                upgradedRobot->setUpgradeCount(this->getUpgradeCount());

                                int robotName_pos = this->robotName().find("_");
                                upgradedRobot->setRobotName(upgradedRobot->id() + this->robotName().substr(robotName_pos));

                                // Replace the robot in the battlefield vector immediately
                                for (auto& robot : battlefield->robots()) {
                                    if (robot == this) {
                                        robot = upgradedRobot;
                                        break; // important: break to avoid double replacement
                                    }
                                }

                                return;
                            } else {
                                cout <<this->id() <<" are now upgraded to ThirtyShotBot!"<< endl;
                                outputLines.push_back(this->id() + " are now upgraded to ThirtyShotBot!");
                                ThirtyShotBot* upgradedRobot = new ThirtyShotBot(this->id(), this->x(), this->y());

                                // Copy state
                                upgradedRobot->setNumOfLives(this->numOfLives());
                                upgradedRobot->setNumOfKills(this->numOfKills());
                                // upgradedRobot->setNumOfShells(this->numOfShell());
                                upgradedRobot->setUpgradeCount(this->getUpgradeCount());

                                int robotName_pos = this->robotName().find("_");
                                upgradedRobot->setRobotName(upgradedRobot->id() + this->robotName().substr(robotName_pos));

                                // Replace the robot in the battlefield vector immediately
                                for (auto& robot : battlefield->robots()) {
                                    if (robot == this) {
                                        robot = upgradedRobot;
                                        break; // important: break to avoid double replacement
                                    }
                                }

                                return;
                            }
                            break;
                        }
                        case 2: {
                            // Randomly choose Seeing upgrade (1 - ScoutBot, 2 - TrackBot)
                            int seeChoice = rand() % 2 + 1;
                            if (seeChoice == 1) {
                                cout <<this->id() <<" are now upgraded to ScoutBot!"<< endl;
                                outputLines.push_back(this->id() + " are now upgraded to ScoutBot!");
                                ScoutBot* upgradedRobot = new ScoutBot(this->id(), this->x(), this->y());

                                // Copy state
                                upgradedRobot->setNumOfLives(this->numOfLives());
                                upgradedRobot->setNumOfKills(this->numOfKills());
                                upgradedRobot->setNumOfShells(this->numOfShell());
                                upgradedRobot->setUpgradeCount(this->getUpgradeCount());

                                int robotName_pos = this->robotName().find("_");
                                upgradedRobot->setRobotName(upgradedRobot->id() + this->robotName().substr(robotName_pos));

                                // Replace the robot in the battlefield vector immediately
                                for (auto& robot : battlefield->robots()) {
                                    if (robot == this) {
                                        robot = upgradedRobot;
                                        break; // important: break to avoid double replacement
                                    }
                                }

                                return;
                            } else {
                                cout <<this->id() <<" are now upgraded to TrackBot!"<< endl;
                                outputLines.push_back(this->id() + " are now upgraded to TrackBot!");
                                TrackBot* upgradedRobot = new TrackBot(this->id(), this->x(), this->y());

                                // Copy state
                                upgradedRobot->setNumOfLives(this->numOfLives());
                                upgradedRobot->setNumOfKills(this->numOfKills());
                                upgradedRobot->setNumOfShells(this->numOfShell());
                                upgradedRobot->setUpgradeCount(this->getUpgradeCount());

                                int robotName_pos = this->robotName().find("_");
                                upgradedRobot->setRobotName(upgradedRobot->id() + this->robotName().substr(robotName_pos));

                                // Replace the robot in the battlefield vector immediately
                                for (auto& robot : battlefield->robots()) {
                                    if (robot == this) {
                                        robot = upgradedRobot;
                                        break; // important: break to avoid double replacement
                                    }
                                }

                                return;
                            }
                            break;
                        }
                        case 3: {
                            // Randomly choose Extra upgrade (1 - HealthBot, 2 - GodBot, 3 - JukeBot)
                            int extraChoice = rand() % 3 + 1;
                            if (extraChoice == 1) {
                                cout << this->id() << " are now upgraded to HealthBot!" << endl;
                                outputLines.push_back(this->id() + " are now upgraded to HealthBot!");
                                HealthBot* upgradedRobot = new HealthBot(this->id(), this->x(), this->y());

                                // Copy state
                                upgradedRobot->setNumOfLives(this->numOfLives());
                                upgradedRobot->setNumOfKills(this->numOfKills());
                                upgradedRobot->setNumOfShells(this->numOfShell());
                                upgradedRobot->setUpgradeCount(this->getUpgradeCount());

                                int robotName_pos = this->robotName().find("_");
                                upgradedRobot->setRobotName(upgradedRobot->id() + this->robotName().substr(robotName_pos));

                                // Replace the robot in the battlefield vector immediately
                                for (auto& robot : battlefield->robots()) {
                                    if (robot == this) {
                                        robot = upgradedRobot;
                                        break; // important: break to avoid double replacement
                                    }
                                }

                                return;
                            } else if (extraChoice == 2) {
                                cout << this->id() << " are now upgraded to GodBot!" << endl;
                                outputLines.push_back(this->id() + " are now upgraded to GodBot!");
                                GodBot* upgradedRobot = new GodBot(this->id(), this->x(), this->y());

                                // Copy state
                                upgradedRobot->setNumOfLives(this->numOfLives());
                                upgradedRobot->setNumOfKills(this->numOfKills());
                                upgradedRobot->setNumOfShells(this->numOfShell());
                                upgradedRobot->setUpgradeCount(this->getUpgradeCount());

                                int robotName_pos = this->robotName().find("_");
                                upgradedRobot->setRobotName(upgradedRobot->id() + this->robotName().substr(robotName_pos));

                                // Replace the robot in the battlefield vector immediately
                                for (auto& robot : battlefield->robots()) {
                                    if (robot == this) {
                                        robot = upgradedRobot;
                                        break; // important: break to avoid double replacement
                                    }
                                }

                                return;
                            } else {
                                cout <<this->id() <<" are now upgraded to JukeBot!"<< endl;
                                outputLines.push_back(this->id() + " are now upgraded to JukeBot!");
                                JukeBot* upgradedRobot = new JukeBot(this->id(), this->x(), this->y());

                                // Copy state
                                upgradedRobot->setNumOfLives(this->numOfLives());
                                upgradedRobot->setNumOfKills(this->numOfKills());
                                upgradedRobot->setNumOfShells(this->numOfShell());
                                upgradedRobot->setUpgradeCount(this->getUpgradeCount());

                                int robotName_pos = this->robotName().find("_");
                                upgradedRobot->setRobotName(upgradedRobot->id() + this->robotName().substr(robotName_pos));

                                // Replace the robot in the battlefield vector immediately
                                for (auto& robot : battlefield->robots()) {
                                    if (robot == this) {
                                        robot = upgradedRobot;
                                        break; // important: break to avoid double replacement
                                    }
                                }

                                return;
                            }
                            break;
                        }
                    }

                } else {
                    cout << "Upgrade limit reached. Can only upgrade twice." << endl;
                    outputLines.push_back(this->id() + " are now upgraded to JukeBot!");
                }
            } else {
                this->decreaseShell();
                cout<<"Shot missed! The enemy robot was not hit."<<endl;
                cout<< this->id() <<" now has "<< this->numOfShell() <<" of shells left!"<<endl;

                outputLines.push_back("Shot missed! The enemy robot was not hit.");
                outputLines.push_back(this->id() + " now has " + to_string(this->numOfShell()) + " of shells left!");
            }
            hit = true;
        }
    }

    if (!hit) {
        this->decreaseShell();
        cout<<"No enemy robot was at the selected location."<<endl;
        cout<< this->id() <<" now has "<< this->numOfShell() <<" of shells left!"<<endl;

        outputLines.push_back("No enemy robot was at the selected location.");
        outputLines.push_back(this->id() + " now has " + to_string(this->numOfShell()) + " of shells left!");

    }
}


// JumpBot Action Logics
void JumpBot::actionThink(Battlefield* battlefield) {
    cout << "JumpBot is thinking about jumping..." << endl;
    outputLines.push_back("JumpBot is thinking about jumping...");

}

void JumpBot::actionLook(Battlefield* battlefield) {
    int currentX = robotPosX;
    int currentY = robotPosY;

    cout << "=== Robot View (3x3 Grid) ===" << endl;
    outputLines.push_back("=== Robot View (3x3 Grid) ===");

    for (int dy = -1; dy <= 1; ++dy) {
        string row = "";
        for (int dx = -1; dx <= 1; ++dx) {
            int x = currentX + dx;
            int y = currentY + dy;

            if (y >= 0 && y < battlefield->BATTLEFIELD_NUM_OF_ROWS() &&
                x >= 0 && x < battlefield->BATTLEFIELD_NUM_OF_COLS()) {

                if (x == currentX && y == currentY) {
                    cout << "[R] ";
                    row += "[R] ";
                } else {
                    string content = battlefield->getCellContent(x, y);
                    if (content.empty()) {
                        cout << "[ ] ";
                        row += "[ ] ";
                    } else {
                        cout << "[" << content.substr(0, 1) << "] ";
                        row += "[" + content.substr(0, 1) + "] ";
                    }
                }
            } else {
                cout << "[#] ";
                row += "[#] ";
            }
        }
        cout << endl;
        outputLines.push_back(row);
    }
}

void JumpBot::actionMove(Battlefield* battlefield) {
    cout << "1. Regular Move\n2. Jump\nChoose action: ";
    outputLines.push_back("1. Regular Move\n2. Jump\nChoose action: ");
    int choice = rand() % 2;


    if (choice == 0) {
        cout << "Regular Move Chosen"<<endl;
        outputLines.push_back("Regular Move Chosen");
        int currentX = robotPosX;
        int currentY = robotPosY;

        vector<string> directions = {"UL", "U", "UR", "L", "C", "R", "DL", "D", "DR"};
        vector<pair<int, int>> directionsMove = {
            {-1, -1}, {0, -1}, {1, -1},
            {-1, 0},  {0, 0},  {1, 0},
            {-1, 1},  {0, 1},  {1, 1}
        };

        cout << "\nAvailable Directions:\n";
        cout << "UL, U, UR\n";
        cout << "L, C, R\n";
        cout << "DL, D, DR\n";

        outputLines.push_back("\nAvailable Directions:");
        outputLines.push_back("UL, U, UR");
        outputLines.push_back("L, C, R");
        outputLines.push_back("DL, D, DR");

        // store valid index of move location  into validIndex
        vector<int> validIndex;
        for (int i = 0; i < directions.size(); ++i) {
            int newX = currentX + directionsMove[i].first;
            int newY = currentY + directionsMove[i].second;

            // Check bounds
            if (newX < 0 || newX >= battlefield->BATTLEFIELD_NUM_OF_COLS() ||
                newY < 0 || newY >= battlefield->BATTLEFIELD_NUM_OF_ROWS())
                continue;

            // Check if destination is occupied
            bool isOccupied = false;
            for (Robot* robot : battlefield->robots()) {
                if (robot->x() == newX && robot->y() == newY) {
                    isOccupied = true;
                    break;
                }
            }

            if (!isOccupied) {
                validIndex.push_back(i); // push back the index of the valid move
            }
        }

        // Handle case when no valid directions
        if (validIndex.empty()) {
            cout << "No valid move directions available\n";
            outputLines.push_back("No valid move directions available");
            return;
        }

        // Randomly pick a valid direction
        int chosenIndex = validIndex[rand() % validIndex.size()];
        string userInput = directions[chosenIndex];

        cout << "Random direction selected: " << userInput << endl;
        outputLines.push_back("Random direction selected: " + userInput);

        // Calculate new position
        int newX = currentX + directionsMove[chosenIndex].first;
        int newY = currentY + directionsMove[chosenIndex].second;

        // Update position
        robotPosX = newX;
        robotPosY = newY;
        battlefield->setCell(currentX, currentY, nullptr);
        battlefield->setCell(newX, newY, this);

        cout << "Moved to (" << newX << "," << newY << ")" << endl;
        outputLines.push_back("Moved to (" + to_string(newX) + "," + to_string(newY) + ")");

    } else if (choice == 1 && jumpCount < MAX_JUMPS) {
        cout << "Jump Move Chosen" << endl;
        outputLines.push_back("Jump Move Chosen");

        int newX, newY;

        // Generate random coordinates
        newX = rand() % battlefield->BATTLEFIELD_NUM_OF_COLS();
        newY = rand() % battlefield->BATTLEFIELD_NUM_OF_ROWS();

        cout << "Enter jump coordinates (X Y): ";
        outputLines.push_back("Enter jump coordinates (X Y): ");
        cout << newX << " " << newY << endl;
        outputLines.push_back(to_string(newX) + " " + to_string(newY));

        // Validate coordinates
        if (newX < 0 || newX >= battlefield->BATTLEFIELD_NUM_OF_COLS() ||
            newY < 0 || newY >= battlefield->BATTLEFIELD_NUM_OF_ROWS()) {
            cout << "Invalid jump location!" << endl;
            outputLines.push_back("Invalid jump location!");
            return;
        }

        // Check if occupied
        for (Robot* robot : battlefield->robots()) {
            if (robot->x() == newX && robot->y() == newY) {
                cout << "Jump location occupied!" << endl;
                outputLines.push_back("Invalid jump location!");
                return;
            }
        }

        // Perform jump
        battlefield->setCell(robotPosX, robotPosY, nullptr);
        robotPosX = newX;
        robotPosY = newY;
        battlefield->setCell(robotPosX, robotPosY, this);
        jumpCount++;

        cout << id_ << " jumped to (" << newX << "," << newY << ")!" << endl;
        outputLines.push_back(id_ + " jumped to (" + to_string(newX) + "," + to_string(newY) + ")!");

    } else {
        cout << "Jump limit reached or invalid choice!" << endl;
        outputLines.push_back( "Invalid jump location!");

    }
}

void JumpBot::actionShoot(Battlefield* battlefield) {

    string CurrentRobotsName = this->robotName();
    cout << "The Current Robot controlled is " << CurrentRobotsName << endl;
    outputLines.push_back("The Current Robot controlled is " + CurrentRobotsName);

    int CurrentRobotsX = this->x();
    int CurrentRobotsY = this->y();

    int targetX, targetY;
    bool validTarget = false;

    int battlefieldWidth = battlefield->BATTLEFIELD_NUM_OF_COLS();
    int battlefieldHeight = battlefield->BATTLEFIELD_NUM_OF_ROWS();

    // Generate list of valid adjacent targets
    vector<pair<int, int>> possibleTargets;
    for (int dx = -1; dx <= 1; dx++) {
        for (int dy = -1; dy <= 1; dy++) {
            if (dx == 0 && dy == 0) continue; // Skip self

            int newX = CurrentRobotsX + dx;
            int newY = CurrentRobotsY + dy;

            // Check if within bounds
            if (newX >= 0 && newX < battlefieldWidth && newY >= 0 && newY < battlefieldHeight) {
                possibleTargets.emplace_back(newX, newY);
            }
        }
    }

    if (!possibleTargets.empty()) {
        int index = rand() % possibleTargets.size();
        targetX = possibleTargets[index].first;
        targetY = possibleTargets[index].second;

        cout << "Random target selected at: " << targetX << " " << targetY << endl;
        outputLines.push_back("Random target selected at: " + to_string(targetX) + " " + to_string(targetY));

        validTarget = true;
    } else {
        cout << "No valid adjacent tiles found for targeting." << endl;
        outputLines.push_back("No valid adjacent tiles found for targeting.");
        return;
    }

    // Shell check
    int ShellLeft = this->numOfShell();
    if (ShellLeft == 0) {
        cout << "Oh no! You ran out of shells! You can't shoot anything! YOU ARE SELF DESTRUCTING!" << endl;
        outputLines.push_back("Oh no! You ran out of shells! You can't shoot anything! YOU ARE SELF DESTRUCTING!");
        this->reduceLives();
        this->setNumOfShells(10);

        return;
    }

    bool hit = false;

    for (Robot* robot : battlefield->robots()) {
        string targetRobotId = robot->id() ;
        int PotentialRobotX = robot->x() ;
        int PotentialRobotY = robot->y() ;
        if (targetX == CurrentRobotsX && targetY == CurrentRobotsY ){
            cout<<"\nYou can't Shoot Yourself"<<endl;
            outputLines.push_back("\nYou can't Shoot Yourself");

            break;
        }
        if (targetX == PotentialRobotX && targetY == PotentialRobotY ){
            if (!robot->canBeTargeted()) {
                cout << "Target is hidden and cannot be hit!" << endl;
                outputLines.push_back("Target is hidden and cannot be hit!");
                return;
            }
            int hitChance = rand() % 100; // number from 0-99
            if (hitChance < 70) { // 70% chance to hit
                cout<<"\nYou've successfully shot an enemy Robot!"<<endl;
                outputLines.push_back("\nYou've successfully shot an enemy Robot!");

                robot->reduceLives();

                if (!robot->isAlive()) {
                    cout << targetRobotId << " has been destroyed!" << endl;
                outputLines.push_back(targetRobotId + " has been destroyed!" );


                    battlefield->setCell(PotentialRobotX, PotentialRobotY, nullptr); // remove robot from battlefield
                    battlefield->clearCell(PotentialRobotX, PotentialRobotY);
                    robot->setX(-1);
                    robot->setY(-2);

                    battlefield->destroyedRobots().push(robot);  // add to destroyed queue

                    // // Optional: If destroyed robots should be removed from active list
                    // auto& robotsVec = battlefield->robots();
                    // auto it = find(robotsVec.begin(), robotsVec.end(), robot);
                    // if (it != robotsVec.end()) {
                    //     robotsVec.erase(it);
                    // }

                } else {
                    cout << targetRobotId << " was hit and is now temporarily inactive!" << endl;
                    outputLines.push_back( targetRobotId + " was hit and is now temporarily inactive!"  );


                    battlefield->setCell(PotentialRobotX, PotentialRobotY, nullptr); // remove temporarily
                    battlefield->clearCell(PotentialRobotX, PotentialRobotY);
                    robot->setX(-1);
                    robot->setY(-1);

                    battlefield->waitingRobots().push({robot, 0}); // add to waiting queue

                    // auto& robotsVec = battlefield->robots();
                    // auto it = find(robotsVec.begin(), robotsVec.end(), robot);
                    // if (it != robotsVec.end()) {
                    //     robotsVec.erase(it);
                    // }
                }


                this->increaseKills();
                this->decreaseShell();
                int lifeLeft = robot->numOfLives();
                cout<< targetRobotId<<" now has "<<lifeLeft<<" of lives left"<<endl;
                outputLines.push_back(targetRobotId + " now has " + to_string(lifeLeft)  + " of lives left");

                cout<< this->id() <<" now has "<< this->numOfKills() <<" of kills!"<<endl;
                outputLines.push_back(this->id() + " now has " + to_string(this->numOfKills()) + " of kills!");

                cout<< this->id() <<" now has "<< this->numOfShell() <<" of shells left!"<<endl;
                outputLines.push_back(this->id() + " now has " + to_string(this->numOfShell()) + " of shells left!");


                // cout << "Remaining Robots: " << endl;
                // for (const auto& r : battlefield->robots()) {
                //     cout << *r << endl;
                // }


                if (this->canUpgrade()) {
                    this->incrementUpgradeCount();

                    cout<<"\n" << this->id() <<" earned an upgrade! A random upgrade is applied!" << endl;
                    outputLines.push_back("\n" + this->id() + " earned an upgrade! A random upgrade is applied!");
                    //cout <<this->id() <<" are now upgraded to ScoutBot!"<< endl;
                // ScoutBot* upgradedRobot = new ScoutBot(this->id().substr(5), this->x(), this->y());
                    // IM LOSING MY SHIT HERE

                    //Randomly choose upgrade category (1 - Moving, 2 - Shooting, 3 - Seeing, 4 - Extras)
                    int choice = rand() % 4 + 1;

                    switch (choice) {
                        // case 1: {
                        //     // Randomly choose Moving upgrade (1 - HideBot, 2 - JumpBot)
                        //     int moveChoice = rand() % 2 + 1;
                        //     if (moveChoice == 1) {
                        //         cout <<this->id() <<" are now upgraded to HideBot!"<< endl;
                        //         outputLines.push_back(this->id() + " are now upgraded to HideBot!");

                        //         HideBot* upgradedRobot = new HideBot(this->id(), this->x(), this->y());

                        //         // Copy state
                        //         upgradedRobot->setNumOfLives(this->numOfLives());
                        //         upgradedRobot->setNumOfKills(this->numOfKills());
                        //         upgradedRobot->setNumOfShells(this->numOfShell());
                        //         upgradedRobot->setUpgradeCount(this->getUpgradeCount());

                        //         int robotName_pos = this->robotName().find("_");
                        //         upgradedRobot->setRobotName(upgradedRobot->id() + this->robotName().substr(robotName_pos));

                        //         // Replace the robot in the battlefield vector immediately
                        //         for (auto& robot : battlefield->robots()) {
                        //             if (robot == this) {
                        //                 robot = upgradedRobot;
                        //                 break; // important: break to avoid double replacement
                        //             }
                        //         }

                        //         return;
                        //     } else {
                        //         cout <<this->id() <<" are now upgraded to JumpBot!"<< endl;
                        //         outputLines.push_back(this->id() + " are now upgraded to JumpBot!");
                        //         JumpBot* upgradedRobot = new JumpBot(this->id(), this->x(), this->y());

                        //         // Copy state
                        //         upgradedRobot->setNumOfLives(this->numOfLives());
                        //         upgradedRobot->setNumOfKills(this->numOfKills());
                        //         upgradedRobot->setNumOfShells(this->numOfShell());
                        //         upgradedRobot->setUpgradeCount(this->getUpgradeCount());

                        //         int robotName_pos = this->robotName().find("_");
                        //         upgradedRobot->setRobotName(upgradedRobot->id() + this->robotName().substr(robotName_pos));

                        //         // Replace the robot in the battlefield vector immediately
                        //         for (auto& robot : battlefield->robots()) {
                        //             if (robot == this) {
                        //                 robot = upgradedRobot;
                        //                 break; // important: break to avoid double replacement
                        //             }
                        //         }

                        //         return;
                        //     }
                        // }
                        case 1: {
                            // Randomly choose Shooting upgrade (1 - LongShotBot, 2 - SemiAutoBot, 3 - ThirtyShotBot)
                            int shootChoice = rand() % 3 + 1;
                            if (shootChoice == 1) {
                                cout <<this->id() <<" are now upgraded to LongShotBot!"<< endl;
                                outputLines.push_back(this->id() + " are now upgraded to LongShotBot!");
                                LongShotBot* upgradedRobot = new LongShotBot(this->id(), this->x(), this->y());

                                // Copy state
                                upgradedRobot->setNumOfLives(this->numOfLives());
                                upgradedRobot->setNumOfKills(this->numOfKills());
                                upgradedRobot->setNumOfShells(this->numOfShell());
                                upgradedRobot->setUpgradeCount(this->getUpgradeCount());

                                int robotName_pos = this->robotName().find("_");
                                upgradedRobot->setRobotName(upgradedRobot->id() + this->robotName().substr(robotName_pos));

                                // Replace the robot in the battlefield vector immediately
                                for (auto& robot : battlefield->robots()) {
                                    if (robot == this) {
                                        robot = upgradedRobot;
                                        break; // important: break to avoid double replacement
                                    }
                                }

                                return;
                            } else if (shootChoice == 2) {
                                cout <<this->id() <<" are now upgraded to SemiAutoBot!"<< endl;
                                outputLines.push_back(this->id() + " are now upgraded to SemiAutoBot!");
                                SemiAutoBot* upgradedRobot = new SemiAutoBot(this->id(), this->x(), this->y());

                                // Copy state
                                upgradedRobot->setNumOfLives(this->numOfLives());
                                upgradedRobot->setNumOfKills(this->numOfKills());
                                upgradedRobot->setNumOfShells(this->numOfShell());
                                upgradedRobot->setUpgradeCount(this->getUpgradeCount());

                                int robotName_pos = this->robotName().find("_");
                                upgradedRobot->setRobotName(upgradedRobot->id() + this->robotName().substr(robotName_pos));

                                // Replace the robot in the battlefield vector immediately
                                for (auto& robot : battlefield->robots()) {
                                    if (robot == this) {
                                        robot = upgradedRobot;
                                        break; // important: break to avoid double replacement
                                    }
                                }

                                return;
                            } else {
                                cout <<this->id() <<" are now upgraded to ThirtyShotBot!"<< endl;
                                outputLines.push_back(this->id() + " are now upgraded to ThirtyShotBot!");
                                ThirtyShotBot* upgradedRobot = new ThirtyShotBot(this->id(), this->x(), this->y());

                                // Copy state
                                upgradedRobot->setNumOfLives(this->numOfLives());
                                upgradedRobot->setNumOfKills(this->numOfKills());
                                // upgradedRobot->setNumOfShells(this->numOfShell());
                                upgradedRobot->setUpgradeCount(this->getUpgradeCount());

                                int robotName_pos = this->robotName().find("_");
                                upgradedRobot->setRobotName(upgradedRobot->id() + this->robotName().substr(robotName_pos));

                                // Replace the robot in the battlefield vector immediately
                                for (auto& robot : battlefield->robots()) {
                                    if (robot == this) {
                                        robot = upgradedRobot;
                                        break; // important: break to avoid double replacement
                                    }
                                }

                                return;
                            }
                            break;
                        }
                        case 2: {
                            // Randomly choose Seeing upgrade (1 - ScoutBot, 2 - TrackBot)
                            int seeChoice = rand() % 2 + 1;
                            if (seeChoice == 1) {
                                cout <<this->id() <<" are now upgraded to ScoutBot!"<< endl;
                                outputLines.push_back(this->id() + " are now upgraded to ScoutBot!");
                                ScoutBot* upgradedRobot = new ScoutBot(this->id(), this->x(), this->y());

                                // Copy state
                                upgradedRobot->setNumOfLives(this->numOfLives());
                                upgradedRobot->setNumOfKills(this->numOfKills());
                                upgradedRobot->setNumOfShells(this->numOfShell());
                                upgradedRobot->setUpgradeCount(this->getUpgradeCount());

                                int robotName_pos = this->robotName().find("_");
                                upgradedRobot->setRobotName(upgradedRobot->id() + this->robotName().substr(robotName_pos));

                                // Replace the robot in the battlefield vector immediately
                                for (auto& robot : battlefield->robots()) {
                                    if (robot == this) {
                                        robot = upgradedRobot;
                                        break; // important: break to avoid double replacement
                                    }
                                }

                                return;
                            } else {
                                cout <<this->id() <<" are now upgraded to TrackBot!"<< endl;
                                outputLines.push_back(this->id() + " are now upgraded to TrackBot!");
                                TrackBot* upgradedRobot = new TrackBot(this->id(), this->x(), this->y());

                                // Copy state
                                upgradedRobot->setNumOfLives(this->numOfLives());
                                upgradedRobot->setNumOfKills(this->numOfKills());
                                upgradedRobot->setNumOfShells(this->numOfShell());
                                upgradedRobot->setUpgradeCount(this->getUpgradeCount());

                                int robotName_pos = this->robotName().find("_");
                                upgradedRobot->setRobotName(upgradedRobot->id() + this->robotName().substr(robotName_pos));

                                // Replace the robot in the battlefield vector immediately
                                for (auto& robot : battlefield->robots()) {
                                    if (robot == this) {
                                        robot = upgradedRobot;
                                        break; // important: break to avoid double replacement
                                    }
                                }

                                return;
                            }
                            break;
                        }
                        case 3: {
                            // Randomly choose Extra upgrade (1 - HealthBot, 2 - GodBot, 3 - JukeBot)
                            int extraChoice = rand() % 3 + 1;
                            if (extraChoice == 1) {
                                cout << this->id() << " are now upgraded to HealthBot!" << endl;
                                outputLines.push_back(this->id() + " are now upgraded to HealthBot!");
                                HealthBot* upgradedRobot = new HealthBot(this->id(), this->x(), this->y());

                                // Copy state
                                upgradedRobot->setNumOfLives(this->numOfLives());
                                upgradedRobot->setNumOfKills(this->numOfKills());
                                upgradedRobot->setNumOfShells(this->numOfShell());
                                upgradedRobot->setUpgradeCount(this->getUpgradeCount());

                                int robotName_pos = this->robotName().find("_");
                                upgradedRobot->setRobotName(upgradedRobot->id() + this->robotName().substr(robotName_pos));

                                // Replace the robot in the battlefield vector immediately
                                for (auto& robot : battlefield->robots()) {
                                    if (robot == this) {
                                        robot = upgradedRobot;
                                        break; // important: break to avoid double replacement
                                    }
                                }

                                return;
                            } else if (extraChoice == 2) {
                                cout << this->id() << " are now upgraded to GodBot!" << endl;
                                outputLines.push_back(this->id() + " are now upgraded to GodBot!");
                                GodBot* upgradedRobot = new GodBot(this->id(), this->x(), this->y());

                                // Copy state
                                upgradedRobot->setNumOfLives(this->numOfLives());
                                upgradedRobot->setNumOfKills(this->numOfKills());
                                upgradedRobot->setNumOfShells(this->numOfShell());
                                upgradedRobot->setUpgradeCount(this->getUpgradeCount());

                                int robotName_pos = this->robotName().find("_");
                                upgradedRobot->setRobotName(upgradedRobot->id() + this->robotName().substr(robotName_pos));

                                // Replace the robot in the battlefield vector immediately
                                for (auto& robot : battlefield->robots()) {
                                    if (robot == this) {
                                        robot = upgradedRobot;
                                        break; // important: break to avoid double replacement
                                    }
                                }

                                return;
                            } else {
                                cout <<this->id() <<" are now upgraded to JukeBot!"<< endl;
                                outputLines.push_back(this->id() + " are now upgraded to JukeBot!");
                                JukeBot* upgradedRobot = new JukeBot(this->id(), this->x(), this->y());

                                // Copy state
                                upgradedRobot->setNumOfLives(this->numOfLives());
                                upgradedRobot->setNumOfKills(this->numOfKills());
                                upgradedRobot->setNumOfShells(this->numOfShell());
                                upgradedRobot->setUpgradeCount(this->getUpgradeCount());

                                int robotName_pos = this->robotName().find("_");
                                upgradedRobot->setRobotName(upgradedRobot->id() + this->robotName().substr(robotName_pos));

                                // Replace the robot in the battlefield vector immediately
                                for (auto& robot : battlefield->robots()) {
                                    if (robot == this) {
                                        robot = upgradedRobot;
                                        break; // important: break to avoid double replacement
                                    }
                                }

                                return;
                            }
                            break;
                        }
                    }

                } else {
                    cout << "Upgrade limit reached. Can only upgrade twice." << endl;
                    outputLines.push_back("Upgrade limit reached. Can only upgrade twice.");

                }
            } else {
                this->decreaseShell();
                cout<<"Shot missed! The enemy robot was not hit."<<endl;
                outputLines.push_back("Shot missed! The enemy robot was not hit.");

                cout<< this->id() <<" now has "<< this->numOfShell() <<" of shells left!"<<endl;
                outputLines.push_back(this->id() + " now has " + to_string(this->numOfShell()) + " of shells left!");

            }
            hit = true;
        }
    }

    if (!hit) {
        this->decreaseShell();
        cout<<"No enemy robot was at the selected location."<<endl;
        outputLines.push_back("No enemy robot was at the selected location.");

        cout<< this->id() <<" now has "<< this->numOfShell() <<" of shells left!"<<endl;
        outputLines.push_back(this->id() + " now has " + to_string(this->numOfShell()) + " of shells left!");

    }
}


// HealthBot Action Logics
void HealthBot::actionThink(Battlefield* battlefield) {
    cout << "HealthBot is thinking about healing..." << endl;
    outputLines.push_back("HealthBot is thinking about healing...");
}

void HealthBot::actionLook(Battlefield* battlefield) {
    int currentX = robotPosX;
    int currentY = robotPosY;

    cout << "=== Robot View (3x3 Grid) ===" << endl;
    outputLines.push_back("=== Robot View (3x3 Grid) ===");

    for (int dy = -1; dy <= 1; ++dy) {
        string row = "";
        for (int dx = -1; dx <= 1; ++dx) {
            int x = currentX + dx;
            int y = currentY + dy;

            if (y >= 0 && y < battlefield->BATTLEFIELD_NUM_OF_ROWS() &&
                x >= 0 && x < battlefield->BATTLEFIELD_NUM_OF_COLS()) {

                if (x == currentX && y == currentY) {
                    cout << "[R] ";
                    row += "[R] ";
                } else {
                    string content = battlefield->getCellContent(x, y);
                    if (content.empty()) {
                        cout << "[ ] ";
                        row += "[ ] ";
                    } else {
                        cout << "[" << content.substr(0, 1) << "] ";
                        row += "[" + content.substr(0, 1) + "] ";
                    }
                }
            } else {
                cout << "[#] ";
                row += "[#] ";
            }
        }
        cout << endl;
        outputLines.push_back(row);
    }
}

void HealthBot::actionMove(Battlefield* battlefield) {
    int currentX = robotPosX;
    int currentY = robotPosY;

    vector<string> directions = {"UL", "U", "UR", "L", "C", "R", "DL", "D", "DR"};
    vector<pair<int, int>> directionsMove = {
        {-1, -1}, {0, -1}, {1, -1},
        {-1, 0},  {0, 0},  {1, 0},
        {-1, 1},  {0, 1},  {1, 1}
    };

    cout << "\nAvailable Directions:\n";
    cout << "UL, U, UR\n";
    cout << "L, C, R\n";
    cout << "DL, D, DR\n";

    outputLines.push_back("\nAvailable Directions:");
    outputLines.push_back("UL, U, UR");
    outputLines.push_back("L, C, R");
    outputLines.push_back("DL, D, DR");

    // store valid index of move location  into validIndex
    vector<int> validIndex;
    for (int i = 0; i < directions.size(); ++i) {
        int newX = currentX + directionsMove[i].first;
        int newY = currentY + directionsMove[i].second;

        // Check bounds
        if (newX < 0 || newX >= battlefield->BATTLEFIELD_NUM_OF_COLS() ||
            newY < 0 || newY >= battlefield->BATTLEFIELD_NUM_OF_ROWS())
            continue;

        // Check if destination is occupied
        bool isOccupied = false;
        for (Robot* robot : battlefield->robots()) {
            if (robot->x() == newX && robot->y() == newY) {
                isOccupied = true;
                break;
            }
        }

        if (!isOccupied) {
            validIndex.push_back(i); // push back the index of the valid move
        }
    }

    // Handle case when no valid directions
    if (validIndex.empty()) {
        cout << "No valid move directions available\n";
        outputLines.push_back("No valid move directions available");
        return;
    }

    // Randomly pick a valid direction
    int chosenIndex = validIndex[rand() % validIndex.size()];
    string userInput = directions[chosenIndex];

    cout << "Random direction selected: " << userInput << endl;
    outputLines.push_back("Random direction selected: " + userInput);

    // Calculate new position
    int newX = currentX + directionsMove[chosenIndex].first;
    int newY = currentY + directionsMove[chosenIndex].second;

    // Update position
    robotPosX = newX;
    robotPosY = newY;
    battlefield->setCell(currentX, currentY, nullptr);
    battlefield->setCell(newX, newY, this);

    cout << "Moved to (" << newX << "," << newY << ")" << endl;
    outputLines.push_back("Moved to (" + to_string(newX) + "," + to_string(newY) + ")");
}

void HealthBot::actionShoot(Battlefield* battlefield) {

    string CurrentRobotsName = this->robotName();
    cout << "The Current Robot controlled is " << CurrentRobotsName << endl;
    outputLines.push_back("The Current Robot controlled is " + CurrentRobotsName);

    int CurrentRobotsX = this->x();
    int CurrentRobotsY = this->y();

    int targetX, targetY;
    bool validTarget = false;

    int battlefieldWidth = battlefield->BATTLEFIELD_NUM_OF_COLS();
    int battlefieldHeight = battlefield->BATTLEFIELD_NUM_OF_ROWS();

    // Generate list of valid adjacent targets
    vector<pair<int, int>> possibleTargets;
    for (int dx = -1; dx <= 1; dx++) {
        for (int dy = -1; dy <= 1; dy++) {
            if (dx == 0 && dy == 0) continue; // Skip self

            int newX = CurrentRobotsX + dx;
            int newY = CurrentRobotsY + dy;

            // Check if within bounds
            if (newX >= 0 && newX < battlefieldWidth && newY >= 0 && newY < battlefieldHeight) {
                possibleTargets.emplace_back(newX, newY);
            }
        }
    }

    if (!possibleTargets.empty()) {
        int index = rand() % possibleTargets.size();
        targetX = possibleTargets[index].first;
        targetY = possibleTargets[index].second;

        cout << "Random target selected at: " << targetX << " " << targetY << endl;
        outputLines.push_back("Random target selected at: " + to_string(targetX) + " " + to_string(targetY));

        validTarget = true;
    } else {
        cout << "No valid adjacent tiles found for targeting." << endl;
        outputLines.push_back("No valid adjacent tiles found for targeting.");
        return;
    }

    // Shell check
    int ShellLeft = this->numOfShell();
    if (ShellLeft == 0) {
        cout << "Oh no! You ran out of shells! You can't shoot anything! YOU ARE SELF DESTRUCTING!" << endl;
        outputLines.push_back("Oh no! You ran out of shells! You can't shoot anything! YOU ARE SELF DESTRUCTING!");
        this->reduceLives();
        this->setNumOfShells(10);

        return;
    }

    bool hit = false;

    for (Robot* robot : battlefield->robots()) {
        string targetRobotId = robot->id() ;
        int PotentialRobotX = robot->x() ;
        int PotentialRobotY = robot->y() ;
        if (targetX == CurrentRobotsX && targetY == CurrentRobotsY ){
            cout<<"\nYou can't Shoot Yourself"<<endl;
            outputLines.push_back("\nYou can't Shoot Yourself");

            break;
        }
        if (targetX == PotentialRobotX && targetY == PotentialRobotY ){
            if (!robot->canBeTargeted()) {
                cout << "Target is hidden and cannot be hit!" << endl;
                outputLines.push_back("Target is hidden and cannot be hit!");
                return;
            }
            int hitChance = rand() % 100; // number from 0-99
            if (hitChance < 70) { // 70% chance to hit
                cout<<"\nYou've successfully shot an enemy Robot!"<<endl;
                outputLines.push_back("\nYou've successfully shot an enemy Robot!");

                robot->reduceLives();

                if (!robot->isAlive()) {
                    cout << targetRobotId << " has been destroyed!" << endl;
                outputLines.push_back(targetRobotId + " has been destroyed!" );


                    battlefield->setCell(PotentialRobotX, PotentialRobotY, nullptr); // remove robot from battlefield
                    battlefield->clearCell(PotentialRobotX, PotentialRobotY);
                    robot->setX(-1);
                    robot->setY(-2);

                    battlefield->destroyedRobots().push(robot);  // add to destroyed queue

                    // // Optional: If destroyed robots should be removed from active list
                    // auto& robotsVec = battlefield->robots();
                    // auto it = find(robotsVec.begin(), robotsVec.end(), robot);
                    // if (it != robotsVec.end()) {
                    //     robotsVec.erase(it);
                    // }

                } else {
                    cout << targetRobotId << " was hit and is now temporarily inactive!" << endl;
                    outputLines.push_back( targetRobotId + " was hit and is now temporarily inactive!"  );


                    battlefield->setCell(PotentialRobotX, PotentialRobotY, nullptr); // remove temporarily
                    battlefield->clearCell(PotentialRobotX, PotentialRobotY);
                    robot->setX(-1);
                    robot->setY(-1);

                    battlefield->waitingRobots().push({robot, 0}); // add to waiting queue

                    // auto& robotsVec = battlefield->robots();
                    // auto it = find(robotsVec.begin(), robotsVec.end(), robot);
                    // if (it != robotsVec.end()) {
                    //     robotsVec.erase(it);
                    // }
                }


                this->increaseKills();
                this->decreaseShell();
                int lifeLeft = robot->numOfLives();
                cout<< targetRobotId<<" now has "<<lifeLeft<<" of lives left"<<endl;
                outputLines.push_back(targetRobotId + " now has " + to_string(lifeLeft)  + " of lives left");

                cout<< this->id() <<" now has "<< this->numOfKills() <<" of kills!"<<endl;
                outputLines.push_back(this->id() + " now has " + to_string(this->numOfKills()) + " of kills!");

                cout<< this->id() <<" now has "<< this->numOfShell() <<" of shells left!"<<endl;
                outputLines.push_back(this->id() + " now has " + to_string(this->numOfShell()) + " of shells left!");


                // cout << "Remaining Robots: " << endl;
                // for (const auto& r : battlefield->robots()) {
                //     cout << *r << endl;
                // }


                if (this->canUpgrade()) {
                    this->incrementUpgradeCount();

                    cout<<"\n" << this->id() <<" earned an upgrade! A random upgrade is applied!" << endl;
                    outputLines.push_back("\n" + this->id() + " earned an upgrade! A random upgrade is applied!");
                    //cout <<this->id() <<" are now upgraded to ScoutBot!"<< endl;
                // ScoutBot* upgradedRobot = new ScoutBot(this->id().substr(5), this->x(), this->y());
                    // IM LOSING MY SHIT HERE

                    //Randomly choose upgrade category (1 - Moving, 2 - Shooting, 3 - Seeing, 4 - Extras)
                    int choice = rand() % 4 + 1;

                    switch (choice) {
                        case 1: {
                            // Randomly choose Moving upgrade (1 - HideBot, 2 - JumpBot)
                            int moveChoice = rand() % 2 + 1;
                            if (moveChoice == 1) {
                                cout <<this->id() <<" are now upgraded to HideBot!"<< endl;
                                outputLines.push_back(this->id() + " are now upgraded to HideBot!");

                                HideBot* upgradedRobot = new HideBot(this->id(), this->x(), this->y());

                                // Copy state
                                upgradedRobot->setNumOfLives(this->numOfLives());
                                upgradedRobot->setNumOfKills(this->numOfKills());
                                upgradedRobot->setNumOfShells(this->numOfShell());
                                upgradedRobot->setUpgradeCount(this->getUpgradeCount());

                                int robotName_pos = this->robotName().find("_");
                                upgradedRobot->setRobotName(upgradedRobot->id() + this->robotName().substr(robotName_pos));

                                // Replace the robot in the battlefield vector immediately
                                for (auto& robot : battlefield->robots()) {
                                    if (robot == this) {
                                        robot = upgradedRobot;
                                        break; // important: break to avoid double replacement
                                    }
                                }

                                return;
                            } else {
                                cout <<this->id() <<" are now upgraded to JumpBot!"<< endl;
                                outputLines.push_back(this->id() + " are now upgraded to JumpBot!");
                                JumpBot* upgradedRobot = new JumpBot(this->id(), this->x(), this->y());

                                // Copy state
                                upgradedRobot->setNumOfLives(this->numOfLives());
                                upgradedRobot->setNumOfKills(this->numOfKills());
                                upgradedRobot->setNumOfShells(this->numOfShell());
                                upgradedRobot->setUpgradeCount(this->getUpgradeCount());

                                int robotName_pos = this->robotName().find("_");
                                upgradedRobot->setRobotName(upgradedRobot->id() + this->robotName().substr(robotName_pos));

                                // Replace the robot in the battlefield vector immediately
                                for (auto& robot : battlefield->robots()) {
                                    if (robot == this) {
                                        robot = upgradedRobot;
                                        break; // important: break to avoid double replacement
                                    }
                                }

                                return;
                            }
                        }
                        case 2: {
                            // Randomly choose Shooting upgrade (1 - LongShotBot, 2 - SemiAutoBot, 3 - ThirtyShotBot)
                            int shootChoice = rand() % 3 + 1;
                            if (shootChoice == 1) {
                                cout <<this->id() <<" are now upgraded to LongShotBot!"<< endl;
                                outputLines.push_back(this->id() + " are now upgraded to LongShotBot!");
                                LongShotBot* upgradedRobot = new LongShotBot(this->id(), this->x(), this->y());

                                // Copy state
                                upgradedRobot->setNumOfLives(this->numOfLives());
                                upgradedRobot->setNumOfKills(this->numOfKills());
                                upgradedRobot->setNumOfShells(this->numOfShell());
                                upgradedRobot->setUpgradeCount(this->getUpgradeCount());

                                int robotName_pos = this->robotName().find("_");
                                upgradedRobot->setRobotName(upgradedRobot->id() + this->robotName().substr(robotName_pos));

                                // Replace the robot in the battlefield vector immediately
                                for (auto& robot : battlefield->robots()) {
                                    if (robot == this) {
                                        robot = upgradedRobot;
                                        break; // important: break to avoid double replacement
                                    }
                                }

                                return;
                            } else if (shootChoice == 2) {
                                cout <<this->id() <<" are now upgraded to SemiAutoBot!"<< endl;
                                outputLines.push_back(this->id() + " are now upgraded to SemiAutoBot!");
                                SemiAutoBot* upgradedRobot = new SemiAutoBot(this->id(), this->x(), this->y());

                                // Copy state
                                upgradedRobot->setNumOfLives(this->numOfLives());
                                upgradedRobot->setNumOfKills(this->numOfKills());
                                upgradedRobot->setNumOfShells(this->numOfShell());
                                upgradedRobot->setUpgradeCount(this->getUpgradeCount());

                                int robotName_pos = this->robotName().find("_");
                                upgradedRobot->setRobotName(upgradedRobot->id() + this->robotName().substr(robotName_pos));

                                // Replace the robot in the battlefield vector immediately
                                for (auto& robot : battlefield->robots()) {
                                    if (robot == this) {
                                        robot = upgradedRobot;
                                        break; // important: break to avoid double replacement
                                    }
                                }

                                return;
                            } else {
                                cout <<this->id() <<" are now upgraded to ThirtyShotBot!"<< endl;
                                outputLines.push_back(this->id() + " are now upgraded to ThirtyShotBot!");
                                ThirtyShotBot* upgradedRobot = new ThirtyShotBot(this->id(), this->x(), this->y());

                                // Copy state
                                upgradedRobot->setNumOfLives(this->numOfLives());
                                upgradedRobot->setNumOfKills(this->numOfKills());
                                // upgradedRobot->setNumOfShells(this->numOfShell());
                                upgradedRobot->setUpgradeCount(this->getUpgradeCount());

                                int robotName_pos = this->robotName().find("_");
                                upgradedRobot->setRobotName(upgradedRobot->id() + this->robotName().substr(robotName_pos));

                                // Replace the robot in the battlefield vector immediately
                                for (auto& robot : battlefield->robots()) {
                                    if (robot == this) {
                                        robot = upgradedRobot;
                                        break; // important: break to avoid double replacement
                                    }
                                }

                                return;
                            }
                            break;
                        }
                        case 3: {
                            // Randomly choose Seeing upgrade (1 - ScoutBot, 2 - TrackBot)
                            int seeChoice = rand() % 2 + 1;
                            if (seeChoice == 1) {
                                cout <<this->id() <<" are now upgraded to ScoutBot!"<< endl;
                                outputLines.push_back(this->id() + " are now upgraded to ScoutBot!");
                                ScoutBot* upgradedRobot = new ScoutBot(this->id(), this->x(), this->y());

                                // Copy state
                                upgradedRobot->setNumOfLives(this->numOfLives());
                                upgradedRobot->setNumOfKills(this->numOfKills());
                                upgradedRobot->setNumOfShells(this->numOfShell());
                                upgradedRobot->setUpgradeCount(this->getUpgradeCount());

                                int robotName_pos = this->robotName().find("_");
                                upgradedRobot->setRobotName(upgradedRobot->id() + this->robotName().substr(robotName_pos));

                                // Replace the robot in the battlefield vector immediately
                                for (auto& robot : battlefield->robots()) {
                                    if (robot == this) {
                                        robot = upgradedRobot;
                                        break; // important: break to avoid double replacement
                                    }
                                }

                                return;
                            } else {
                                cout <<this->id() <<" are now upgraded to TrackBot!"<< endl;
                                outputLines.push_back(this->id() + " are now upgraded to TrackBot!");
                                TrackBot* upgradedRobot = new TrackBot(this->id(), this->x(), this->y());

                                // Copy state
                                upgradedRobot->setNumOfLives(this->numOfLives());
                                upgradedRobot->setNumOfKills(this->numOfKills());
                                upgradedRobot->setNumOfShells(this->numOfShell());
                                upgradedRobot->setUpgradeCount(this->getUpgradeCount());

                                int robotName_pos = this->robotName().find("_");
                                upgradedRobot->setRobotName(upgradedRobot->id() + this->robotName().substr(robotName_pos));

                                // Replace the robot in the battlefield vector immediately
                                for (auto& robot : battlefield->robots()) {
                                    if (robot == this) {
                                        robot = upgradedRobot;
                                        break; // important: break to avoid double replacement
                                    }
                                }

                                return;
                            }
                            break;
                        }
                        // case 3: {
                        //     // Randomly choose Extra upgrade (1 - HealthBot, 2 - GodBot, 3 - JukeBot)
                        //     int extraChoice = rand() % 3 + 1;
                        //     if (extraChoice == 1) {
                        //         cout << this->id() << " are now upgraded to HealthBot!" << endl;
                        //         outputLines.push_back(this->id() + " are now upgraded to HealthBot!");
                        //         HealthBot* upgradedRobot = new HealthBot(this->id(), this->x(), this->y());

                        //         // Copy state
                        //         upgradedRobot->setNumOfLives(this->numOfLives());
                        //         upgradedRobot->setNumOfKills(this->numOfKills());
                        //         upgradedRobot->setNumOfShells(this->numOfShell());
                        //         upgradedRobot->setUpgradeCount(this->getUpgradeCount());

                        //         int robotName_pos = this->robotName().find("_");
                        //         upgradedRobot->setRobotName(upgradedRobot->id() + this->robotName().substr(robotName_pos));

                        //         // Replace the robot in the battlefield vector immediately
                        //         for (auto& robot : battlefield->robots()) {
                        //             if (robot == this) {
                        //                 robot = upgradedRobot;
                        //                 break; // important: break to avoid double replacement
                        //             }
                        //         }

                        //         return;
                        //     } else if (extraChoice == 2) {
                        //         cout << this->id() << " are now upgraded to GodBot!" << endl;
                        //         outputLines.push_back(this->id() + " are now upgraded to GodBot!");
                        //         GodBot* upgradedRobot = new GodBot(this->id(), this->x(), this->y());

                        //         // Copy state
                        //         upgradedRobot->setNumOfLives(this->numOfLives());
                        //         upgradedRobot->setNumOfKills(this->numOfKills());
                        //         upgradedRobot->setNumOfShells(this->numOfShell());
                        //         upgradedRobot->setUpgradeCount(this->getUpgradeCount());

                        //         int robotName_pos = this->robotName().find("_");
                        //         upgradedRobot->setRobotName(upgradedRobot->id() + this->robotName().substr(robotName_pos));

                        //         // Replace the robot in the battlefield vector immediately
                        //         for (auto& robot : battlefield->robots()) {
                        //             if (robot == this) {
                        //                 robot = upgradedRobot;
                        //                 break; // important: break to avoid double replacement
                        //             }
                        //         }

                        //         return;
                        //     } else {
                        //         cout <<this->id() <<" are now upgraded to JukeBot!"<< endl;
                        //         outputLines.push_back(this->id() + " are now upgraded to JukeBot!");
                        //         JukeBot* upgradedRobot = new JukeBot(this->id(), this->x(), this->y());

                        //         // Copy state
                        //         upgradedRobot->setNumOfLives(this->numOfLives());
                        //         upgradedRobot->setNumOfKills(this->numOfKills());
                        //         upgradedRobot->setNumOfShells(this->numOfShell());
                        //         upgradedRobot->setUpgradeCount(this->getUpgradeCount());

                        //         int robotName_pos = this->robotName().find("_");
                        //         upgradedRobot->setRobotName(upgradedRobot->id() + this->robotName().substr(robotName_pos));

                        //         // Replace the robot in the battlefield vector immediately
                        //         for (auto& robot : battlefield->robots()) {
                        //             if (robot == this) {
                        //                 robot = upgradedRobot;
                        //                 break; // important: break to avoid double replacement
                        //             }
                        //         }

                        //         return;
                        //     }
                        //     break;
                        // }
                    }

                } else {
                    cout << "Upgrade limit reached. Can only upgrade twice." << endl;
                }
            } else {
                this->decreaseShell();
                cout<<"Shot missed! The enemy robot was not hit."<<endl;
                cout<< this->id() <<" now has "<< this->numOfShell() <<" of shells left!"<<endl;
            }
            hit = true;
        }
    }

    if (!hit) {
        this->decreaseShell();
        cout<<"No enemy robot was at the selected location."<<endl;
        cout<< this->id() <<" now has "<< this->numOfShell() <<" of shells left!"<<endl;
    }
}



// JukeBot Action Logics
void JukeBot::actionThink(Battlefield* battlefield) {
    cout << "JukeBot is thinking about juking..." << endl;
    outputLines.push_back("JukeBot is thinking about juking...");

}

void JukeBot::actionLook(Battlefield* battlefield) {
    int currentX = robotPosX;
    int currentY = robotPosY;

    cout << "=== Robot View (3x3 Grid) ===" << endl;
    outputLines.push_back("=== Robot View (3x3 Grid) ===");

    for (int dy = -1; dy <= 1; ++dy) {
        string row = "";
        for (int dx = -1; dx <= 1; ++dx) {
            int x = currentX + dx;
            int y = currentY + dy;

            if (y >= 0 && y < battlefield->BATTLEFIELD_NUM_OF_ROWS() &&
                x >= 0 && x < battlefield->BATTLEFIELD_NUM_OF_COLS()) {

                if (x == currentX && y == currentY) {
                    cout << "[R] ";
                    row += "[R] ";
                } else {
                    string content = battlefield->getCellContent(x, y);
                    if (content.empty()) {
                        cout << "[ ] ";
                        row += "[ ] ";
                    } else {
                        cout << "[" << content.substr(0, 1) << "] ";
                        row += "[" + content.substr(0, 1) + "] ";
                    }
                }
            } else {
                cout << "[#] ";
                row += "[#] ";
            }
        }
        cout << endl;
        outputLines.push_back(row);
    }
}

void JukeBot::actionMove(Battlefield* battlefield) {
    int currentX = robotPosX;
    int currentY = robotPosY;

    vector<string> directions = {"UL", "U", "UR", "L", "C", "R", "DL", "D", "DR"};
    vector<pair<int, int>> directionsMove = {
        {-1, -1}, {0, -1}, {1, -1},
        {-1, 0},  {0, 0},  {1, 0},
        {-1, 1},  {0, 1},  {1, 1}
    };

    cout << "\nAvailable Directions:\n";
    cout << "UL, U, UR\n";
    cout << "L, C, R\n";
    cout << "DL, D, DR\n";

    outputLines.push_back("\nAvailable Directions:");
    outputLines.push_back("UL, U, UR");
    outputLines.push_back("L, C, R");
    outputLines.push_back("DL, D, DR");

    // store valid index of move location  into validIndex
    vector<int> validIndex;
    for (int i = 0; i < directions.size(); ++i) {
        int newX = currentX + directionsMove[i].first;
        int newY = currentY + directionsMove[i].second;

        // Check bounds
        if (newX < 0 || newX >= battlefield->BATTLEFIELD_NUM_OF_COLS() ||
            newY < 0 || newY >= battlefield->BATTLEFIELD_NUM_OF_ROWS())
            continue;

        // Check if destination is occupied
        bool isOccupied = false;
        for (Robot* robot : battlefield->robots()) {
            if (robot->x() == newX && robot->y() == newY) {
                isOccupied = true;
                break;
            }
        }

        if (!isOccupied) {
            validIndex.push_back(i); // push back the index of the valid move
        }
    }

    // Handle case when no valid directions
    if (validIndex.empty()) {
        cout << "No valid move directions available\n";
        outputLines.push_back("No valid move directions available");
        return;
    }

    // Randomly pick a valid direction
    int chosenIndex = validIndex[rand() % validIndex.size()];
    string userInput = directions[chosenIndex];

    cout << "Random direction selected: " << userInput << endl;
    outputLines.push_back("Random direction selected: " + userInput);

    // Calculate new position
    int newX = currentX + directionsMove[chosenIndex].first;
    int newY = currentY + directionsMove[chosenIndex].second;

    // Update position
    robotPosX = newX;
    robotPosY = newY;
    battlefield->setCell(currentX, currentY, nullptr);
    battlefield->setCell(newX, newY, this);

    cout << "Moved to (" << newX << "," << newY << ")" << endl;
    outputLines.push_back("Moved to (" + to_string(newX) + "," + to_string(newY) + ")");
}

void JukeBot::actionShoot(Battlefield* battlefield) {

    string CurrentRobotsName = this->robotName();
    cout << "The Current Robot controlled is " << CurrentRobotsName << endl;
    outputLines.push_back("The Current Robot controlled is " + CurrentRobotsName);

    int CurrentRobotsX = this->x();
    int CurrentRobotsY = this->y();

    int targetX, targetY;
    bool validTarget = false;

    int battlefieldWidth = battlefield->BATTLEFIELD_NUM_OF_COLS();
    int battlefieldHeight = battlefield->BATTLEFIELD_NUM_OF_ROWS();

    // Generate list of valid adjacent targets
    vector<pair<int, int>> possibleTargets;
    for (int dx = -1; dx <= 1; dx++) {
        for (int dy = -1; dy <= 1; dy++) {
            if (dx == 0 && dy == 0) continue; // Skip self

            int newX = CurrentRobotsX + dx;
            int newY = CurrentRobotsY + dy;

            // Check if within bounds
            if (newX >= 0 && newX < battlefieldWidth && newY >= 0 && newY < battlefieldHeight) {
                possibleTargets.emplace_back(newX, newY);
            }
        }
    }

    if (!possibleTargets.empty()) {
        int index = rand() % possibleTargets.size();
        targetX = possibleTargets[index].first;
        targetY = possibleTargets[index].second;

        cout << "Random target selected at: " << targetX << " " << targetY << endl;
        outputLines.push_back("Random target selected at: " + to_string(targetX) + " " + to_string(targetY));

        validTarget = true;
    } else {
        cout << "No valid adjacent tiles found for targeting." << endl;
        outputLines.push_back("No valid adjacent tiles found for targeting.");
        return;
    }

    // Shell check
    int ShellLeft = this->numOfShell();
    if (ShellLeft == 0) {
        cout << "Oh no! You ran out of shells! You can't shoot anything! YOU ARE SELF DESTRUCTING!" << endl;
        outputLines.push_back("Oh no! You ran out of shells! You can't shoot anything! YOU ARE SELF DESTRUCTING!");
        this->reduceLives();
        this->setNumOfShells(10);

        return;
    }

    bool hit = false;

    for (Robot* robot : battlefield->robots()) {
        string targetRobotId = robot->id() ;
        int PotentialRobotX = robot->x() ;
        int PotentialRobotY = robot->y() ;
        if (targetX == CurrentRobotsX && targetY == CurrentRobotsY ){
            cout<<"\nYou can't Shoot Yourself"<<endl;
            outputLines.push_back("\nYou can't Shoot Yourself");

            break;
        }
        if (targetX == PotentialRobotX && targetY == PotentialRobotY ){
            if (!robot->canBeTargeted()) {
                cout << "Target is hidden and cannot be hit!" << endl;
                outputLines.push_back("Target is hidden and cannot be hit!");
                return;
            }
            int hitChance = rand() % 100; // number from 0-99
            if (hitChance < 70) { // 70% chance to hit
                cout<<"\nYou've successfully shot an enemy Robot!"<<endl;
                outputLines.push_back("\nYou've successfully shot an enemy Robot!");

                robot->reduceLives();

                if (!robot->isAlive()) {
                    cout << targetRobotId << " has been destroyed!" << endl;
                outputLines.push_back(targetRobotId + " has been destroyed!" );


                    battlefield->setCell(PotentialRobotX, PotentialRobotY, nullptr); // remove robot from battlefield
                    battlefield->clearCell(PotentialRobotX, PotentialRobotY);
                    robot->setX(-1);
                    robot->setY(-2);

                    battlefield->destroyedRobots().push(robot);  // add to destroyed queue

                    // // Optional: If destroyed robots should be removed from active list
                    // auto& robotsVec = battlefield->robots();
                    // auto it = find(robotsVec.begin(), robotsVec.end(), robot);
                    // if (it != robotsVec.end()) {
                    //     robotsVec.erase(it);
                    // }

                } else {
                    cout << targetRobotId << " was hit and is now temporarily inactive!" << endl;
                    outputLines.push_back( targetRobotId + " was hit and is now temporarily inactive!"  );


                    battlefield->setCell(PotentialRobotX, PotentialRobotY, nullptr); // remove temporarily
                    battlefield->clearCell(PotentialRobotX, PotentialRobotY);
                    robot->setX(-1);
                    robot->setY(-1);

                    battlefield->waitingRobots().push({robot, 0}); // add to waiting queue

                    // auto& robotsVec = battlefield->robots();
                    // auto it = find(robotsVec.begin(), robotsVec.end(), robot);
                    // if (it != robotsVec.end()) {
                    //     robotsVec.erase(it);
                    // }
                }


                this->increaseKills();
                this->decreaseShell();
                int lifeLeft = robot->numOfLives();
                cout<< targetRobotId<<" now has "<<lifeLeft<<" of lives left"<<endl;
                outputLines.push_back(targetRobotId + " now has " + to_string(lifeLeft)  + " of lives left");

                cout<< this->id() <<" now has "<< this->numOfKills() <<" of kills!"<<endl;
                outputLines.push_back(this->id() + " now has " + to_string(this->numOfKills()) + " of kills!");

                cout<< this->id() <<" now has "<< this->numOfShell() <<" of shells left!"<<endl;
                outputLines.push_back(this->id() + " now has " + to_string(this->numOfShell()) + " of shells left!");


                // cout << "Remaining Robots: " << endl;
                // for (const auto& r : battlefield->robots()) {
                //     cout << *r << endl;
                // }


                if (this->canUpgrade()) {
                    this->incrementUpgradeCount();

                    cout<<"\n" << this->id() <<" earned an upgrade! A random upgrade is applied!" << endl;
                    outputLines.push_back("\n" + this->id() + " earned an upgrade! A random upgrade is applied!");
                    //cout <<this->id() <<" are now upgraded to ScoutBot!"<< endl;
                // ScoutBot* upgradedRobot = new ScoutBot(this->id().substr(5), this->x(), this->y());
                    // IM LOSING MY SHIT HERE

                    //Randomly choose upgrade category (1 - Moving, 2 - Shooting, 3 - Seeing, 4 - Extras)
                    int choice = rand() % 4 + 1;

                    switch (choice) {
                        case 1: {
                            // Randomly choose Moving upgrade (1 - HideBot, 2 - JumpBot)
                            int moveChoice = rand() % 2 + 1;
                            if (moveChoice == 1) {
                                cout <<this->id() <<" are now upgraded to HideBot!"<< endl;
                                outputLines.push_back(this->id() + " are now upgraded to HideBot!");

                                HideBot* upgradedRobot = new HideBot(this->id(), this->x(), this->y());

                                // Copy state
                                upgradedRobot->setNumOfLives(this->numOfLives());
                                upgradedRobot->setNumOfKills(this->numOfKills());
                                upgradedRobot->setNumOfShells(this->numOfShell());
                                upgradedRobot->setUpgradeCount(this->getUpgradeCount());

                                int robotName_pos = this->robotName().find("_");
                                upgradedRobot->setRobotName(upgradedRobot->id() + this->robotName().substr(robotName_pos));

                                // Replace the robot in the battlefield vector immediately
                                for (auto& robot : battlefield->robots()) {
                                    if (robot == this) {
                                        robot = upgradedRobot;
                                        break; // important: break to avoid double replacement
                                    }
                                }

                                return;
                            } else {
                                cout <<this->id() <<" are now upgraded to JumpBot!"<< endl;
                                outputLines.push_back(this->id() + " are now upgraded to JumpBot!");
                                JumpBot* upgradedRobot = new JumpBot(this->id(), this->x(), this->y());

                                // Copy state
                                upgradedRobot->setNumOfLives(this->numOfLives());
                                upgradedRobot->setNumOfKills(this->numOfKills());
                                upgradedRobot->setNumOfShells(this->numOfShell());
                                upgradedRobot->setUpgradeCount(this->getUpgradeCount());

                                int robotName_pos = this->robotName().find("_");
                                upgradedRobot->setRobotName(upgradedRobot->id() + this->robotName().substr(robotName_pos));

                                // Replace the robot in the battlefield vector immediately
                                for (auto& robot : battlefield->robots()) {
                                    if (robot == this) {
                                        robot = upgradedRobot;
                                        break; // important: break to avoid double replacement
                                    }
                                }

                                return;
                            }
                        }
                        case 2: {
                            // Randomly choose Shooting upgrade (1 - LongShotBot, 2 - SemiAutoBot, 3 - ThirtyShotBot)
                            int shootChoice = rand() % 3 + 1;
                            if (shootChoice == 1) {
                                cout <<this->id() <<" are now upgraded to LongShotBot!"<< endl;
                                outputLines.push_back(this->id() + " are now upgraded to LongShotBot!");
                                LongShotBot* upgradedRobot = new LongShotBot(this->id(), this->x(), this->y());

                                // Copy state
                                upgradedRobot->setNumOfLives(this->numOfLives());
                                upgradedRobot->setNumOfKills(this->numOfKills());
                                upgradedRobot->setNumOfShells(this->numOfShell());
                                upgradedRobot->setUpgradeCount(this->getUpgradeCount());

                                int robotName_pos = this->robotName().find("_");
                                upgradedRobot->setRobotName(upgradedRobot->id() + this->robotName().substr(robotName_pos));

                                // Replace the robot in the battlefield vector immediately
                                for (auto& robot : battlefield->robots()) {
                                    if (robot == this) {
                                        robot = upgradedRobot;
                                        break; // important: break to avoid double replacement
                                    }
                                }

                                return;
                            } else if (shootChoice == 2) {
                                cout <<this->id() <<" are now upgraded to SemiAutoBot!"<< endl;
                                outputLines.push_back(this->id() + " are now upgraded to SemiAutoBot!");
                                SemiAutoBot* upgradedRobot = new SemiAutoBot(this->id(), this->x(), this->y());

                                // Copy state
                                upgradedRobot->setNumOfLives(this->numOfLives());
                                upgradedRobot->setNumOfKills(this->numOfKills());
                                upgradedRobot->setNumOfShells(this->numOfShell());
                                upgradedRobot->setUpgradeCount(this->getUpgradeCount());

                                int robotName_pos = this->robotName().find("_");
                                upgradedRobot->setRobotName(upgradedRobot->id() + this->robotName().substr(robotName_pos));

                                // Replace the robot in the battlefield vector immediately
                                for (auto& robot : battlefield->robots()) {
                                    if (robot == this) {
                                        robot = upgradedRobot;
                                        break; // important: break to avoid double replacement
                                    }
                                }

                                return;
                            } else {
                                cout <<this->id() <<" are now upgraded to ThirtyShotBot!"<< endl;
                                outputLines.push_back(this->id() + " are now upgraded to ThirtyShotBot!");
                                ThirtyShotBot* upgradedRobot = new ThirtyShotBot(this->id(), this->x(), this->y());

                                // Copy state
                                upgradedRobot->setNumOfLives(this->numOfLives());
                                upgradedRobot->setNumOfKills(this->numOfKills());
                                // upgradedRobot->setNumOfShells(this->numOfShell());
                                upgradedRobot->setUpgradeCount(this->getUpgradeCount());

                                int robotName_pos = this->robotName().find("_");
                                upgradedRobot->setRobotName(upgradedRobot->id() + this->robotName().substr(robotName_pos));

                                // Replace the robot in the battlefield vector immediately
                                for (auto& robot : battlefield->robots()) {
                                    if (robot == this) {
                                        robot = upgradedRobot;
                                        break; // important: break to avoid double replacement
                                    }
                                }

                                return;
                            }
                            break;
                        }
                        case 3: {
                            // Randomly choose Seeing upgrade (1 - ScoutBot, 2 - TrackBot)
                            int seeChoice = rand() % 2 + 1;
                            if (seeChoice == 1) {
                                cout <<this->id() <<" are now upgraded to ScoutBot!"<< endl;
                                outputLines.push_back(this->id() + " are now upgraded to ScoutBot!");
                                ScoutBot* upgradedRobot = new ScoutBot(this->id(), this->x(), this->y());

                                // Copy state
                                upgradedRobot->setNumOfLives(this->numOfLives());
                                upgradedRobot->setNumOfKills(this->numOfKills());
                                upgradedRobot->setNumOfShells(this->numOfShell());
                                upgradedRobot->setUpgradeCount(this->getUpgradeCount());

                                int robotName_pos = this->robotName().find("_");
                                upgradedRobot->setRobotName(upgradedRobot->id() + this->robotName().substr(robotName_pos));

                                // Replace the robot in the battlefield vector immediately
                                for (auto& robot : battlefield->robots()) {
                                    if (robot == this) {
                                        robot = upgradedRobot;
                                        break; // important: break to avoid double replacement
                                    }
                                }

                                return;
                            } else {
                                cout <<this->id() <<" are now upgraded to TrackBot!"<< endl;
                                outputLines.push_back(this->id() + " are now upgraded to TrackBot!");
                                TrackBot* upgradedRobot = new TrackBot(this->id(), this->x(), this->y());

                                // Copy state
                                upgradedRobot->setNumOfLives(this->numOfLives());
                                upgradedRobot->setNumOfKills(this->numOfKills());
                                upgradedRobot->setNumOfShells(this->numOfShell());
                                upgradedRobot->setUpgradeCount(this->getUpgradeCount());

                                int robotName_pos = this->robotName().find("_");
                                upgradedRobot->setRobotName(upgradedRobot->id() + this->robotName().substr(robotName_pos));

                                // Replace the robot in the battlefield vector immediately
                                for (auto& robot : battlefield->robots()) {
                                    if (robot == this) {
                                        robot = upgradedRobot;
                                        break; // important: break to avoid double replacement
                                    }
                                }

                                return;
                            }
                            break;
                        }
                        // case 3: {
                        //     // Randomly choose Extra upgrade (1 - HealthBot, 2 - GodBot, 3 - JukeBot)
                        //     int extraChoice = rand() % 3 + 1;
                        //     if (extraChoice == 1) {
                        //         cout << this->id() << " are now upgraded to HealthBot!" << endl;
                        //         outputLines.push_back(this->id() + " are now upgraded to HealthBot!");
                        //         HealthBot* upgradedRobot = new HealthBot(this->id(), this->x(), this->y());

                        //         // Copy state
                        //         upgradedRobot->setNumOfLives(this->numOfLives());
                        //         upgradedRobot->setNumOfKills(this->numOfKills());
                        //         upgradedRobot->setNumOfShells(this->numOfShell());
                        //         upgradedRobot->setUpgradeCount(this->getUpgradeCount());

                        //         int robotName_pos = this->robotName().find("_");
                        //         upgradedRobot->setRobotName(upgradedRobot->id() + this->robotName().substr(robotName_pos));

                        //         // Replace the robot in the battlefield vector immediately
                        //         for (auto& robot : battlefield->robots()) {
                        //             if (robot == this) {
                        //                 robot = upgradedRobot;
                        //                 break; // important: break to avoid double replacement
                        //             }
                        //         }

                        //         return;
                        //     } else if (extraChoice == 2) {
                        //         cout << this->id() << " are now upgraded to GodBot!" << endl;
                        //         outputLines.push_back(this->id() + " are now upgraded to GodBot!");
                        //         GodBot* upgradedRobot = new GodBot(this->id(), this->x(), this->y());

                        //         // Copy state
                        //         upgradedRobot->setNumOfLives(this->numOfLives());
                        //         upgradedRobot->setNumOfKills(this->numOfKills());
                        //         upgradedRobot->setNumOfShells(this->numOfShell());
                        //         upgradedRobot->setUpgradeCount(this->getUpgradeCount());

                        //         int robotName_pos = this->robotName().find("_");
                        //         upgradedRobot->setRobotName(upgradedRobot->id() + this->robotName().substr(robotName_pos));

                        //         // Replace the robot in the battlefield vector immediately
                        //         for (auto& robot : battlefield->robots()) {
                        //             if (robot == this) {
                        //                 robot = upgradedRobot;
                        //                 break; // important: break to avoid double replacement
                        //             }
                        //         }

                        //         return;
                        //     } else {
                        //         cout <<this->id() <<" are now upgraded to JukeBot!"<< endl;
                        //         outputLines.push_back(this->id() + " are now upgraded to JukeBot!");
                        //         JukeBot* upgradedRobot = new JukeBot(this->id(), this->x(), this->y());

                        //         // Copy state
                        //         upgradedRobot->setNumOfLives(this->numOfLives());
                        //         upgradedRobot->setNumOfKills(this->numOfKills());
                        //         upgradedRobot->setNumOfShells(this->numOfShell());
                        //         upgradedRobot->setUpgradeCount(this->getUpgradeCount());

                        //         int robotName_pos = this->robotName().find("_");
                        //         upgradedRobot->setRobotName(upgradedRobot->id() + this->robotName().substr(robotName_pos));

                        //         // Replace the robot in the battlefield vector immediately
                        //         for (auto& robot : battlefield->robots()) {
                        //             if (robot == this) {
                        //                 robot = upgradedRobot;
                        //                 break; // important: break to avoid double replacement
                        //             }
                        //         }

                        //         return;
                        //     }
                        //     break;
                        // }
                    }

                } else {
                    cout << "Upgrade limit reached. Can only upgrade twice." << endl;
                }
            } else {
                this->decreaseShell();
                cout<<"Shot missed! The enemy robot was not hit."<<endl;
                cout<< this->id() <<" now has "<< this->numOfShell() <<" of shells left!"<<endl;
            }
            hit = true;
        }
    }

    if (!hit) {
        this->decreaseShell();
        cout<<"No enemy robot was at the selected location."<<endl;
        cout<< this->id() <<" now has "<< this->numOfShell() <<" of shells left!"<<endl;
    }
}


// GodBot Action Logics
void GodBot::actionThink(Battlefield* battlefield) {
    cout << "GodBot is thinking about divine intervention..." << endl;
    outputLines.push_back("GodBot is thinking about divine intervention...");
}

// Sees
void GodBot::actionLook(Battlefield* battlefield) {

    int currentX = robotPosX;
    int currentY = robotPosY;

    cout << "=== Robot View (3x3 Grid) ===" << endl;
    outputLines.push_back("=== Robot View (3x3 Grid) ===");

    for (int dy = -1; dy <= 1; ++dy) {
        string row = "";
        for (int dx = -1; dx <= 1; ++dx) {
            int x = currentX + dx;
            int y = currentY + dy;

            if (y >= 0 && y < battlefield->BATTLEFIELD_NUM_OF_ROWS() &&
                x >= 0 && x < battlefield->BATTLEFIELD_NUM_OF_COLS()) {

                if (x == currentX && y == currentY) {
                    cout << "[R] ";
                    row += "[R] ";
                } else {
                    string content = battlefield->getCellContent(x, y);
                    if (content.empty()) {
                        cout << "[ ] ";
                        row += "[ ] ";
                    } else {
                        cout << "[" << content.substr(0, 1) << "] ";
                        row += "[" + content.substr(0, 1) + "] ";
                    }
                }
            } else {
                cout << "[#] ";
                row += "[#] ";
            }
        }
        cout << endl;
        outputLines.push_back(row);
    }
}

// Jump Anywhere
void GodBot::actionMove(Battlefield* battlefield) {
    int currentX = robotPosX;
    int currentY = robotPosY;

    vector<string> directions = {"UL", "U", "UR", "L", "C", "R", "DL", "D", "DR"};
    vector<pair<int, int>> directionsMove = {
        {-1, -1}, {0, -1}, {1, -1},
        {-1, 0},  {0, 0},  {1, 0},
        {-1, 1},  {0, 1},  {1, 1}
    };

    cout << "\nAvailable Directions:\n";
    cout << "UL, U, UR\n";
    cout << "L, C, R\n";
    cout << "DL, D, DR\n";

    outputLines.push_back("\nAvailable Directions:");
    outputLines.push_back("UL, U, UR");
    outputLines.push_back("L, C, R");
    outputLines.push_back("DL, D, DR");

    // store valid index of move location  into validIndex
    vector<int> validIndex;
    for (int i = 0; i < directions.size(); ++i) {
        int newX = currentX + directionsMove[i].first;
        int newY = currentY + directionsMove[i].second;

        // Check bounds
        if (newX < 0 || newX >= battlefield->BATTLEFIELD_NUM_OF_COLS() ||
            newY < 0 || newY >= battlefield->BATTLEFIELD_NUM_OF_ROWS())
            continue;

        // Check if destination is occupied
        bool isOccupied = false;
        for (Robot* robot : battlefield->robots()) {
            if (robot->x() == newX && robot->y() == newY) {
                isOccupied = true;
                break;
            }
        }

        if (!isOccupied) {
            validIndex.push_back(i); // push back the index of the valid move
        }
    }

    // Handle case when no valid directions
    if (validIndex.empty()) {
        cout << "No valid move directions available\n";
        outputLines.push_back("No valid move directions available");
        return;
    }

    // Randomly pick a valid direction
    int chosenIndex = validIndex[rand() % validIndex.size()];
    string userInput = directions[chosenIndex];

    cout << "Random direction selected: " << userInput << endl;
    outputLines.push_back("Random direction selected: " + userInput);

    // Calculate new position
    int newX = currentX + directionsMove[chosenIndex].first;
    int newY = currentY + directionsMove[chosenIndex].second;

    // Update position
    robotPosX = newX;
    robotPosY = newY;
    battlefield->setCell(currentX, currentY, nullptr);
    battlefield->setCell(newX, newY, this);

    cout << "Moved to (" << newX << "," << newY << ")" << endl;
    outputLines.push_back("Moved to (" + to_string(newX) + "," + to_string(newY) + ")");
}

// LongShot
void GodBot::actionShoot(Battlefield* battlefield) {
    string CurrentRobotsName = this->robotName();
    cout << "The Current Robot controlled is " << CurrentRobotsName << endl;
    outputLines.push_back("The Current Robot controlled is " + CurrentRobotsName);

    int CurrentRobotsX = this->x();
    int CurrentRobotsY = this->y();

    int targetX, targetY;
    bool validTarget = false;

    int battlefieldWidth = battlefield->BATTLEFIELD_NUM_OF_COLS();
    int battlefieldHeight = battlefield->BATTLEFIELD_NUM_OF_ROWS();


    vector<pair<int, int>> possibleTargets;
    for (int dx = -3; dx <= 3; dx++) {
        for (int dy = -3; dy <= 3; dy++) {
            if (dx == 0 && dy == 0) continue; // Skip self
            if (abs(dx) + abs(dy) > 3) continue;

            int newX = CurrentRobotsX + dx;
            int newY = CurrentRobotsY + dy;

            // Check if within bounds
            if (newX >= 0 && newX < battlefieldWidth && newY >= 0 && newY < battlefieldHeight) {
                possibleTargets.emplace_back(newX, newY);
            }
        }
    }

    if (!possibleTargets.empty()) {
        int index = rand() % possibleTargets.size();
        targetX = possibleTargets[index].first;
        targetY = possibleTargets[index].second;

        cout << "Random target selected at: " << targetX << " " << targetY << endl;
        outputLines.push_back("Random target selected at: " + to_string(targetX) + " " + to_string(targetY));

        validTarget = true;
    } else {
        cout << "No valid tiles found for targeting." << endl;
        outputLines.push_back("No valid tiles found for targeting.");
        return; // Exit early since there's no valid target
    }

    // Shell check
    int ShellLeft = this->numOfShell();
    if (ShellLeft == 0) {
        cout << "Oh no! You ran out of shells! You can't shoot anything! YOU ARE SELF DESTRUCTING!" << endl;
        outputLines.push_back("Oh no! You ran out of shells! You can't shoot anything! YOU ARE SELF DESTRUCTING!");
        this->reduceLives();
        this->setNumOfShells(30);
        return;
    }

    bool hit = false;

    for (Robot* robot : battlefield->robots()) {
        string targetRobotId = robot->id() ;
        int PotentialRobotX = robot->x() ;
        int PotentialRobotY = robot->y() ;
        if (targetX == CurrentRobotsX && targetY == CurrentRobotsY ){
            cout<<"\nYou can't Shoot Yourself"<<endl;
            outputLines.push_back("\nYou can't Shoot Yourself");

            break;
        }
        if (targetX == PotentialRobotX && targetY == PotentialRobotY ){
            if (!robot->canBeTargeted()) {
                cout << "Target is hidden and cannot be hit!" << endl;
                outputLines.push_back("Target is hidden and cannot be hit!");
                return;
            }
            int hitChance = rand() % 100; // number from 0-99
            if (hitChance < 70) { // 70% chance to hit
                cout<<"\nYou've successfully shot an enemy Robot!"<<endl;
                outputLines.push_back("\nYou've successfully shot an enemy Robot!" );

                robot->reduceLives();

                if (!robot->isAlive()) {
                    cout << targetRobotId << " has been destroyed!" << endl;
                    outputLines.push_back(targetRobotId + " has been destroyed!" );


                    battlefield->setCell(PotentialRobotX, PotentialRobotY, nullptr); // remove robot from battlefield
                    battlefield->clearCell(PotentialRobotX, PotentialRobotY);
                    robot->setX(-1);
                    robot->setY(-2);

                    battlefield->destroyedRobots().push(robot);  // add to destroyed queue

                    // // Optional: If destroyed robots should be removed from active list
                    // auto& robotsVec = battlefield->robots();
                    // auto it = find(robotsVec.begin(), robotsVec.end(), robot);
                    // if (it != robotsVec.end()) {
                    //     robotsVec.erase(it);
                    // }

                } else {
                    cout << targetRobotId << " was hit and is now temporarily inactive!" << endl;
                    outputLines.push_back(targetRobotId + " was hit and is now temporarily inactive!" );


                    battlefield->setCell(PotentialRobotX, PotentialRobotY, nullptr); // remove temporarily
                    battlefield->clearCell(PotentialRobotX, PotentialRobotY);
                    robot->setX(-1);
                    robot->setY(-1);

                    battlefield->waitingRobots().push({robot, 0}); // add to waiting queue

                    // auto& robotsVec = battlefield->robots();
                    // auto it = find(robotsVec.begin(), robotsVec.end(), robot);
                    // if (it != robotsVec.end()) {
                    //     robotsVec.erase(it);
                    // }
                }


                this->increaseKills();
                this->decreaseShell();
                int lifeLeft = robot->numOfLives();
                cout<< targetRobotId<<" now has "<<lifeLeft<<" of lives left"<<endl;
                cout<< this->id() <<" now has "<< this->numOfKills() <<" of kills!"<<endl;
                cout<< this->id() <<" now has "<< this->numOfShell() <<" of shells left!"<<endl;

                outputLines.push_back(targetRobotId + " now has " + to_string(lifeLeft) + " of lives left");
                outputLines.push_back(this->id() + " now has " + to_string(this->numOfKills())  + " of kills!");
                outputLines.push_back(this->id() + " now has " + to_string(this->numOfShell()) + " of shells left!");


                // cout << "Remaining Robots: " << endl;
                // for (const auto& r : battlefield->robots()) {
                //     cout << *r << endl;
                // }


                if (this->canUpgrade()) {
                    this->incrementUpgradeCount();

                    cout<<"\n" << this->id() <<" earned an upgrade! A random upgrade is applied!" << endl;
                    outputLines.push_back("\n" + this->id() + " earned an upgrade! A random upgrade is applied!");

                    cout <<this->id() << " are now upgraded to ScoutBot!"<< endl;
                    outputLines.push_back(this->id() + " are now upgraded to ScoutBot!");

                    ScoutBot* upgradedRobot = new ScoutBot(this->id().substr(5), this->x(), this->y());
                    // IM LOSING MY SHIT HERE

                    //Randomly choose upgrade category (1 - Moving, 2 - Seeing, 3 - Extras)
                    int choice = rand() % 3 + 1;

                    switch (choice) {
                        case 1: {
                            // Randomly choose Moving upgrade (1 - HideBot, 2 - JumpBot)
                            int moveChoice = rand() % 2 + 1;
                            if (moveChoice == 1) {
                                cout <<this->id() <<" are now upgraded to HideBot!"<< endl;
                                outputLines.push_back(this->id() + " are now upgraded to HideBot!");

                                HideBot* upgradedRobot = new HideBot(this->id(), this->x(), this->y());

                                // Copy state
                                upgradedRobot->setNumOfLives(this->numOfLives());
                                upgradedRobot->setNumOfKills(this->numOfKills());
                                upgradedRobot->setNumOfShells(this->numOfShell());
                                upgradedRobot->setUpgradeCount(this->getUpgradeCount());

                                int robotName_pos = this->robotName().find("_");
                                upgradedRobot->setRobotName(upgradedRobot->id() + this->robotName().substr(robotName_pos));

                                // Replace the robot in the battlefield vector immediately
                                for (auto& robot : battlefield->robots()) {
                                    if (robot == this) {
                                        robot = upgradedRobot;
                                        break; // important: break to avoid double replacement
                                    }
                                }

                                return;
                            } else {
                                cout <<this->id() <<" are now upgraded to JumpBot!"<< endl;
                                outputLines.push_back(this->id() + " are now upgraded to JumpBot!");
                                JumpBot* upgradedRobot = new JumpBot(this->id(), this->x(), this->y());

                                // Copy state
                                upgradedRobot->setNumOfLives(this->numOfLives());
                                upgradedRobot->setNumOfKills(this->numOfKills());
                                upgradedRobot->setNumOfShells(this->numOfShell());
                                upgradedRobot->setUpgradeCount(this->getUpgradeCount());

                                int robotName_pos = this->robotName().find("_");
                                upgradedRobot->setRobotName(upgradedRobot->id() + this->robotName().substr(robotName_pos));

                                // Replace the robot in the battlefield vector immediately
                                for (auto& robot : battlefield->robots()) {
                                    if (robot == this) {
                                        robot = upgradedRobot;
                                        break; // important: break to avoid double replacement
                                    }
                                }

                                return;
                            }
                        }
                        case 2: {
                            // Randomly choose Shooting upgrade (1 - LongShotBot, 2 - SemiAutoBot, 3 - ThirtyShotBot)
                            int shootChoice = rand() % 3 + 1;
                            if (shootChoice == 1) {
                                cout <<this->id() <<" are now upgraded to LongShotBot!"<< endl;
                                outputLines.push_back(this->id() + " are now upgraded to LongShotBot!");
                                LongShotBot* upgradedRobot = new LongShotBot(this->id(), this->x(), this->y());

                                // Copy state
                                upgradedRobot->setNumOfLives(this->numOfLives());
                                upgradedRobot->setNumOfKills(this->numOfKills());
                                upgradedRobot->setNumOfShells(this->numOfShell());
                                upgradedRobot->setUpgradeCount(this->getUpgradeCount());

                                int robotName_pos = this->robotName().find("_");
                                upgradedRobot->setRobotName(upgradedRobot->id() + this->robotName().substr(robotName_pos));

                                // Replace the robot in the battlefield vector immediately
                                for (auto& robot : battlefield->robots()) {
                                    if (robot == this) {
                                        robot = upgradedRobot;
                                        break; // important: break to avoid double replacement
                                    }
                                }

                                return;
                            } else if (shootChoice == 2) {
                                cout <<this->id() <<" are now upgraded to SemiAutoBot!"<< endl;
                                outputLines.push_back(this->id() + " are now upgraded to SemiAutoBot!");
                                SemiAutoBot* upgradedRobot = new SemiAutoBot(this->id(), this->x(), this->y());

                                // Copy state
                                upgradedRobot->setNumOfLives(this->numOfLives());
                                upgradedRobot->setNumOfKills(this->numOfKills());
                                upgradedRobot->setNumOfShells(this->numOfShell());
                                upgradedRobot->setUpgradeCount(this->getUpgradeCount());

                                int robotName_pos = this->robotName().find("_");
                                upgradedRobot->setRobotName(upgradedRobot->id() + this->robotName().substr(robotName_pos));

                                // Replace the robot in the battlefield vector immediately
                                for (auto& robot : battlefield->robots()) {
                                    if (robot == this) {
                                        robot = upgradedRobot;
                                        break; // important: break to avoid double replacement
                                    }
                                }

                                return;
                            } else {
                                cout <<this->id() <<" are now upgraded to ThirtyShotBot!"<< endl;
                                outputLines.push_back(this->id() + " are now upgraded to ThirtyShotBot!");
                                ThirtyShotBot* upgradedRobot = new ThirtyShotBot(this->id(), this->x(), this->y());

                                // Copy state
                                upgradedRobot->setNumOfLives(this->numOfLives());
                                upgradedRobot->setNumOfKills(this->numOfKills());
                                // upgradedRobot->setNumOfShells(this->numOfShell());
                                upgradedRobot->setUpgradeCount(this->getUpgradeCount());

                                int robotName_pos = this->robotName().find("_");
                                upgradedRobot->setRobotName(upgradedRobot->id() + this->robotName().substr(robotName_pos));

                                // Replace the robot in the battlefield vector immediately
                                for (auto& robot : battlefield->robots()) {
                                    if (robot == this) {
                                        robot = upgradedRobot;
                                        break; // important: break to avoid double replacement
                                    }
                                }

                                return;
                            }
                            break;
                        }
                        case 3: {
                            // Randomly choose Seeing upgrade (1 - ScoutBot, 2 - TrackBot)
                            int seeChoice = rand() % 2 + 1;
                            if (seeChoice == 1) {
                                cout <<this->id() <<" are now upgraded to ScoutBot!"<< endl;
                                outputLines.push_back(this->id() + " are now upgraded to ScoutBot!");
                                ScoutBot* upgradedRobot = new ScoutBot(this->id(), this->x(), this->y());

                                // Copy state
                                upgradedRobot->setNumOfLives(this->numOfLives());
                                upgradedRobot->setNumOfKills(this->numOfKills());
                                upgradedRobot->setNumOfShells(this->numOfShell());
                                upgradedRobot->setUpgradeCount(this->getUpgradeCount());

                                int robotName_pos = this->robotName().find("_");
                                upgradedRobot->setRobotName(upgradedRobot->id() + this->robotName().substr(robotName_pos));

                                // Replace the robot in the battlefield vector immediately
                                for (auto& robot : battlefield->robots()) {
                                    if (robot == this) {
                                        robot = upgradedRobot;
                                        break; // important: break to avoid double replacement
                                    }
                                }

                                return;
                            } else {
                                cout <<this->id() <<" are now upgraded to TrackBot!"<< endl;
                                outputLines.push_back(this->id() + " are now upgraded to TrackBot!");
                                TrackBot* upgradedRobot = new TrackBot(this->id(), this->x(), this->y());

                                // Copy state
                                upgradedRobot->setNumOfLives(this->numOfLives());
                                upgradedRobot->setNumOfKills(this->numOfKills());
                                upgradedRobot->setNumOfShells(this->numOfShell());
                                upgradedRobot->setUpgradeCount(this->getUpgradeCount());

                                int robotName_pos = this->robotName().find("_");
                                upgradedRobot->setRobotName(upgradedRobot->id() + this->robotName().substr(robotName_pos));

                                // Replace the robot in the battlefield vector immediately
                                for (auto& robot : battlefield->robots()) {
                                    if (robot == this) {
                                        robot = upgradedRobot;
                                        break; // important: break to avoid double replacement
                                    }
                                }

                                return;
                            }
                            break;
                        }
                        // case 3: {
                        //     // Randomly choose Extra upgrade (1 - HealthBot, 2 - GodBot, 3 - JukeBot)
                        //     int extraChoice = rand() % 3 + 1;
                        //     if (extraChoice == 1) {
                        //         cout << this->id() << " are now upgraded to HealthBot!" << endl;
                        //         outputLines.push_back(this->id() + " are now upgraded to HealthBot!");
                        //         HealthBot* upgradedRobot = new HealthBot(this->id(), this->x(), this->y());

                        //         // Copy state
                        //         upgradedRobot->setNumOfLives(this->numOfLives());
                        //         upgradedRobot->setNumOfKills(this->numOfKills());
                        //         upgradedRobot->setNumOfShells(this->numOfShell());
                        //         upgradedRobot->setUpgradeCount(this->getUpgradeCount());

                        //         int robotName_pos = this->robotName().find("_");
                        //         upgradedRobot->setRobotName(upgradedRobot->id() + this->robotName().substr(robotName_pos));

                        //         // Replace the robot in the battlefield vector immediately
                        //         for (auto& robot : battlefield->robots()) {
                        //             if (robot == this) {
                        //                 robot = upgradedRobot;
                        //                 break; // important: break to avoid double replacement
                        //             }
                        //         }

                        //         return;
                        //     } else if (extraChoice == 2) {
                        //         cout << this->id() << " are now upgraded to GodBot!" << endl;
                        //         outputLines.push_back(this->id() + " are now upgraded to GodBot!");
                        //         GodBot* upgradedRobot = new GodBot(this->id(), this->x(), this->y());

                        //         // Copy state
                        //         upgradedRobot->setNumOfLives(this->numOfLives());
                        //         upgradedRobot->setNumOfKills(this->numOfKills());
                        //         upgradedRobot->setNumOfShells(this->numOfShell());
                        //         upgradedRobot->setUpgradeCount(this->getUpgradeCount());

                        //         int robotName_pos = this->robotName().find("_");
                        //         upgradedRobot->setRobotName(upgradedRobot->id() + this->robotName().substr(robotName_pos));

                        //         // Replace the robot in the battlefield vector immediately
                        //         for (auto& robot : battlefield->robots()) {
                        //             if (robot == this) {
                        //                 robot = upgradedRobot;
                        //                 break; // important: break to avoid double replacement
                        //             }
                        //         }

                        //         return;
                        //     } else {
                        //         cout <<this->id() <<" are now upgraded to JukeBot!"<< endl;
                        //         outputLines.push_back(this->id() + " are now upgraded to JukeBot!");
                        //         JukeBot* upgradedRobot = new JukeBot(this->id(), this->x(), this->y());

                        //         // Copy state
                        //         upgradedRobot->setNumOfLives(this->numOfLives());
                        //         upgradedRobot->setNumOfKills(this->numOfKills());
                        //         upgradedRobot->setNumOfShells(this->numOfShell());
                        //         upgradedRobot->setUpgradeCount(this->getUpgradeCount());

                        //         int robotName_pos = this->robotName().find("_");
                        //         upgradedRobot->setRobotName(upgradedRobot->id() + this->robotName().substr(robotName_pos));

                        //         // Replace the robot in the battlefield vector immediately
                        //         for (auto& robot : battlefield->robots()) {
                        //             if (robot == this) {
                        //                 robot = upgradedRobot;
                        //                 break; // important: break to avoid double replacement
                        //             }
                        //         }

                        //         return;
                        //     }
                        //     break;
                        // }
                    }

                } else {
                    cout << "Upgrade limit reached. Can only upgrade twice." << endl;
                    outputLines.push_back("Upgrade limit reached. Can only upgrade twice.");
                }
            } else {
                this->decreaseShell();
                cout<<"Shot missed! The enemy robot was not hit."<<endl;
                outputLines.push_back("Upgrade limit reached. Can only upgrade twice.");

                cout<< this->id() <<" now has "<< this->numOfShell() <<" of shells left!"<<endl;
                outputLines.push_back(this->id() + " now has " + to_string(this->numOfShell())  + " of shells left!");

            }
            hit = true;
        }
    }

    if (!hit) {
        this->decreaseShell();
        cout<<"No enemy robot was at the selected location."<<endl;
        outputLines.push_back("No enemy robot was at the selected location.");

        cout<< this->id() <<" now has "<< this->numOfShell() <<" of shells left!"<<endl;
        outputLines.push_back(this->id() + " now has " + to_string(this->numOfShell())  + " of shells left!");

    }
}



// Main Function
int main() {
    srand(time(0));
    //srand(2422122441020/1000);

    Battlefield battlefield;

    // read the file
    string inputFile = "fileInput1.txt";
    battlefield.readFile(inputFile);

    // Get total number of turns and robot list
    int totalTurns = battlefield.turns();
    vector<Robot*>& robots = battlefield.robots();
    int robotCount = robots.size();

    cout << "Total turns: " << totalTurns << endl;
    outputLines.push_back("Total turns: " + to_string(totalTurns));

    int currentTurn = 0;
    int actualTurnsPlayed = 0;

    while (currentTurn < totalTurns) {
        actualTurnsPlayed++;
        int robotIndex = (currentTurn % robotCount);
        Robot* currentRobot = robots[robotIndex];

        // condition to end game when only 1 survivor left
        int aliveCount = 0;
        Robot* lastSurvivor = nullptr;
        for (Robot* robot : robots) {
            if (robot->numOfLives() != 0 && robot->x() != -1 && robot->y() != -2) {
                aliveCount++;
                lastSurvivor = robot;
            }
        }

        // If only one robot remains, declare it the winner and break
        if (aliveCount == 1) {
            cout << "Early victory! Only one robot remains!" << endl;
            outputLines.push_back("Early victory! Only one robot remains!");
            auto winner = lastSurvivor;
            break;  // Exit the loop immediately
        }

        // If no robot remains, declare it the winner and break
        if (aliveCount == 0) {
            cout << "\nAll robots are either destroyed or waiting. Game over.\n";
            outputLines.push_back("All robots are either destroyed or waiting. Game over.");
            // cout << "Total turns played: " << currentTurn << endl;
            // outputLines.push_back("Total turns played: " + to_string(currentTurn));
            break;
        }

        //////////////////////////////////////////////////////////////////////////

        // // Check if all robots are inactive
        // bool allRobotsInactive = true;
        // for (Robot* robot : robots) {
        //     if (robot->x() != -1 && robot->y() != -2) {
        //         allRobotsInactive = false;
        //         break;
        //     }
        // }

        // // check if all bots are in destroyed
        // if (allRobotsInactive) {
        //     cout << "\nAll robots are either destroyed or waiting. Game over.\n";
        //     outputLines.push_back("All robots are either destroyed or waiting. Game over.");
        //     cout << "Total turns played: " << currentTurn << endl;
        //     outputLines.push_back("Total turns played: " + to_string(currentTurn));
        //     break;
        // }

        //////////////////////////////////////////////////////////////////////////

        //  check if current bots has no lives left
        if (currentRobot->x() == -1 && currentRobot->y() == -1) {
            cout << "\nSkipping Turns, Robot " << currentRobot->robotName() << " is still in waiting queue.\n" << endl;
            outputLines.push_back("Skipping Turns, Robot " + currentRobot->robotName() + " is still in waiting queue.");
            currentTurn++;
            continue;
        }

        // OR

        //  check if current bots has no lives left
        if (!currentRobot->isAlive()) {
            currentTurn++;
            cout << "\nSkipping Turns, Robot " << currentRobot->robotName() << " is destroyed and out of the game!\n" << endl;
            outputLines.push_back("Skipping Turns, Robot " + currentRobot->robotName() + " is destroyed and out of the game!");
            continue;
        }

        ///////////////////////////////////////////////////////////////////////////

        //  check if current bots is in waiting queue
        if (currentRobot->x() == -1 && currentRobot->y() == -2) {
            cout << "\nSkipping Turns, Robot " << currentRobot->robotName() << " is destroyed and out of the game!\n" << endl;
            outputLines.push_back("Skipping Turns, Robot " + currentRobot->robotName() + " is destroyed and out of the game!");
            currentTurn++;
            continue;
        }

        //  check if all rounds are played
        if (currentTurn == totalTurns - 1) {
            // Reached last allowed turn
            cout << "All Rounds Played" << endl;
            outputLines.push_back("All Rounds Played");
            break;
        }


        battlefield.placeRobots();
        battlefield.displayBattlefield();
        battlefield.updateWaitingRobots();

        cout << "Turn " << currentTurn + 1 << endl;
        outputLines.push_back("Turn " + to_string(currentTurn + 1));

        cout << "Robot Info: " << *currentRobot << endl;
        ostringstream oss;
        oss.str("");
        oss.clear();
        oss << *currentRobot;
        outputLines.push_back("Robot Info: " + oss.str());

        cout << "Robot Type: " << currentRobot->robotType() << endl;
        outputLines.push_back("Robot Type: " + currentRobot->robotType());

        currentRobot->actions(&battlefield);
        currentTurn++;


    }

    int highestKills = 0;
    Robot* winner = nullptr;

    for (Robot* robot : robots) {
        if (robot->numOfKills() > highestKills && robot->numOfLives() > 0) {
            highestKills = robot->numOfKills();
            winner = robot;
        }
    }

    if (winner != nullptr) {
        cout << "The Ultimate Winner is ......." << endl;
        outputLines.push_back("The Ultimate Winner is .......");

        cout << winner->robotType() << " " << winner->id() << " with " << winner->numOfKills() << " kills and " << winner->numOfLives() << " lives left with rounds played :"<< actualTurnsPlayed  << endl;
        outputLines.push_back(winner->robotType() + " " + winner->id() + " with " + to_string(winner->numOfKills()) + " kills and " + to_string(winner->numOfLives()) + " lives left with rounds played " + to_string(actualTurnsPlayed ));

    } else {
        cout << "No winner (no robot survived or had kills)." << endl;
        outputLines.push_back("No winner (no robot survived or had kills).");
    }

    string outputFile = "fileOutput" + inputFile.substr(9);
    ofstream outFile(outputFile);
    for (const string& line : outputLines) {
        outFile << line << endl;
    }
    outFile.close();

    return 0;
}
