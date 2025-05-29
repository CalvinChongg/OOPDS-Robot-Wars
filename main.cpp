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
#include <algorithm>
 
using namespace std;


class Battlefield;



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

    // for limiting upgrade to 2 per robots
    int getUpgradeCount() const {
        return upgradeCount;
    }

    void incrementUpgradeCount() {
        if (upgradeCount < 2)
            upgradeCount++;
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

    virtual void actionThink(Battlefield* battlefield) {
        // Implement the logic for thinking robot actions here
    }

    virtual void actionLook(Battlefield* battlefield);
    
    virtual void actionShoot(Battlefield* battlefield);

    virtual void actionMove(Battlefield* battlefield);

    virtual void actions (Battlefield* battlefield) {
        int choice;

        cout << "Actions:" << endl;
        cout << "1." << robotType_ << " actionMove" << endl;
        cout << "2." << robotType_ << " actionShoot" << endl;
        cout << "3." << robotType_ << " actionLook" << endl;
        cout << "4." << robotType_ << " actionThink" << endl;
        cout << "5." << robotType_ << " actionSkip" << endl;

        cout << "Please choose your action: ";
        cin >> choice;

        switch (choice) {
            case 1:
                actionMove(battlefield);
                break;
            case 2:
                actionShoot(battlefield);
                break;
            case 3:
                actionLook(battlefield);
                break;
            case 4:
                actionThink(battlefield);
                break;
            case 5:
                cout << "Skipping actions." << endl;
                break;
            default:
                cout << "Invalid choice. Please try again." << endl;
        }



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

    void updateWaitingRobots() {
        int size = waitingRobots_.size();
        for (int i = 0; i < size; ++i) {
            auto [robot, turnsWaited] = waitingRobots_.front();
            waitingRobots_.pop();

            if (turnsWaited + 1 >= 5) {
                // Find random empty position
                int newX, newY;
                bool positionFound = false;
                
                // Try random positions first
                for (int attempt = 0; attempt < 100; attempt++) {
                    newX = rand() % BATTLEFIELD_NUM_OF_COLS_;
                    newY = rand() % BATTLEFIELD_NUM_OF_ROWS_;
                    
                    if (isCellEmpty(newX, newY)) {
                        positionFound = true;
                        break;
                    }
                }
                
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
                    
                    cout << robot->id() << " has waited 5 turns and is returning at (" 
                        << newX << "," << newY << ")\n";
                    robots_.push_back(robot);
                } else {
                    cout << "No empty space found for " << robot->id() 
                        << ", keeping in waiting queue\n";
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
                
                srand(time(0));

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
                robot->setRobotType(tag);
                robot->setRobotName(robotName);
                robots_.push_back(robot);
                
            }

        }

        

        GameFile.close();
    }

    // void placeCurrentRobot(Robot* robot) {
    //     if (robot == nullptr) {
    //         cout << "No robot to place!" << endl;
    //         return;
    //     }

    //     string robotID = robot->id();
    //     int x = robot->x();
    //     int y = robot->y();

    //     // Check if the robot is already placed
    //     // for (const auto& row : battlefield_) {
    //     //     for (const auto& cell : row) {
    //     //         if (cell == robotID) {
    //     //             cout << "Robot " << robotID << " is already placed at (" << x << ", " << y << ")." << endl;
    //     //             return;
    //     //         }
    //     //     }
    //     // }

    //     // Place the robot on the battlefield
    //     for (Robot* robots : robots_) { 
    //         if (robotID == robots->id()) {
    //              if (y < battlefield_.size() && x < battlefield_[y].size()) {
    //                 setCell(x, y, robot);
    //             } else {
    //                 cout << "Robot " << robotID << " is out of bounds!" << endl;
    //                 exit(1);
    //             }
    //         }
    //     }

    //     // if (y < battlefield_.size() && x < battlefield_[y].size()) {
    //     //     setCell(x, y, robot);
    //     // } else {
    //     //     cout << "Robot " << robotID << " is out of bounds!" << endl;
    //     //     exit(1);
    //     // }
    // }

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

    void displayBattlefield(Robot* robot, int currentTurn, int robotCount) {
        int robotIndex = (currentTurn % robotCount);
        Robot* currentRobot = robots_[robotIndex];

        int robotX = currentRobot->x();
        int robotY = currentRobot->y();

        cout << "Display Battlefield (Current Robot Only)" << endl;

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

            for (int j = 0; j < battlefield_[i].size(); j++) {
                if (i == robotY && j == robotX) {
                    cout << "|" << left << setfill(' ') << setw(4) << currentRobot->id();
                } else {
                    cout << "|" << "    ";
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



// Seeing Upgrades
class ScoutBot: public ThinkingRobot, public SeeingRobot, 
                public ShootingRobot, public MovingRobot 
{
public:
    ScoutBot(string id = "", int x = -1, int y = -1) {
        id_ = id;
        robotType_ = "ScoutBot";
        robotPosX = x;
        robotPosY = y;
        robotName_ = "SB" + id;
    }

    virtual ~ScoutBot() {}

    void setLocations(int x, int y) override {
        robotPosX = x;
        robotPosY = y;
    }

    void actionThink(Battlefield* battlefield) override {
        cout << "ScoutBot is analyzing the battlefield..." << endl;
    }

    void actionLook(Battlefield* battlefield) override {
        cout << "=== ScoutBot View (Full Battlefield) ===" << endl;
        
        // Display column headers
        cout << "     ";
        for (int x = 0; x < battlefield->BATTLEFIELD_NUM_OF_COLS(); ++x) {
            cout << "  " << right << setfill('0') << setw(2) << x << " ";
        }
        cout << endl;

        for (int y = 0; y < battlefield->BATTLEFIELD_NUM_OF_ROWS(); ++y) {
            // Display row border
            cout << "   ";
            for (int x = 0; x < battlefield->BATTLEFIELD_NUM_OF_COLS(); ++x) {
                cout << "+----";
            }
            cout << "+" << endl;

            // Display row number
            cout << " " << right << setfill('0') << setw(2) << y << " ";
            
            // Display row content
            for (int x = 0; x < battlefield->BATTLEFIELD_NUM_OF_COLS(); ++x) {
                string content = battlefield->getCellContent(x, y);
                if (x == robotPosX && y == robotPosY) {
                    cout << "|" << left << setfill(' ') << setw(4) << "[R]"; // Mark robot itself
                } else if (content.empty()) {
                    cout << "|    ";
                } else {
                    cout << "|" << left << setfill(' ') << setw(4) << content;
                }
            }
            cout << "|" << endl;
        }

        // Display bottom border
        cout << "   ";
        for (int x = 0; x < battlefield->BATTLEFIELD_NUM_OF_COLS(); ++x) {
            cout << "+----";
        }
        cout << "+" << endl;
    }

    void actionMove(Battlefield* battlefield) override {
        int currentX = robotPosX;
        int currentY = robotPosY;

        vector<string> directions = {"UL", "U", "UR", "L", "C", "R", "DL", "D", "DR"};
        vector<pair<int, int>> directionsMove = {
            {-1, -1}, {0, -1}, {1, -1},
            {-1, 0}, {0, 0}, {1, 0},
            {-1, 1}, {0, 1}, {1, 1}
        };

        // Show available directions
        cout << "\nAvailable Directions:\n";
        cout << "UL, U, UR\n";
        cout << "L, C, R\n";
        cout << "DL, D, DR\n";

        // Enter direction input
        string userInput;
        cout << "Enter direction: ";
        cin >> userInput;

        // Find matching direction
        int moveIndex = -1;
        for (int i = 0; i < directions.size(); i++) {
            if (directions[i] == userInput) {
                moveIndex = i;
                break;
            }
        }

        // Check invalid input 
        if (moveIndex == -1) {
            cout << "Invalid move direction\n";
            return;
        }

        // Calculate new position
        int newX = currentX;
        int newY = currentY;

        if (userInput != "C") {
            newX += directionsMove[moveIndex].first;
            newY += directionsMove[moveIndex].second;
        }

        // Check if moves within the boundary
        if (newX < 0 || newX >= battlefield->BATTLEFIELD_NUM_OF_COLS() ||
            newY < 0 || newY >= battlefield->BATTLEFIELD_NUM_OF_ROWS()) {
            cout << "Cannot move outside battlefield\n";
            return;
        }

        // Check if destination is occupied
        for (Robot* robot : battlefield->robots()) { 
            int PotentialRobotX = robot->x();
            int PotentialRobotY = robot->y();

            if (newX == PotentialRobotX && newY == PotentialRobotY) {
                cout << "Destination occupied" << endl;
                return;  // Stop and do not move if the destination is occupied
            }
        }

        // If we reach here, it means the destination is free
        robotPosX = newX;
        robotPosY = newY;

        cout << "Moved to (" << newX << "," << newY << ")" << endl;
    }

    void actionShoot(Battlefield* battlefield) override {
        // Implement the logic for shooting robot actions here
        cout << "GenericRobot actionShoot" << endl;

        string CurrentRobotsName = this->robotName();
        cout<<"The Current Robot controlled is "<<CurrentRobotsName<<endl;

        int CurrentRobotsX = this->x();
        int CurrentRobotsY = this->y();

        int targetX, targetY;

        bool validTarget = false;

        int battlefieldWidth = battlefield->BATTLEFIELD_NUM_OF_COLS();    
        int battlefieldHeight = battlefield->BATTLEFIELD_NUM_OF_ROWS();  

        do {
            cout << "Enter your target coordinates (X Y): ";
            cin >> targetX >> targetY;

            // calculate distance between target and self
            int dx = abs(targetX - CurrentRobotsX);
            int dy = abs(targetY - CurrentRobotsY);

            // check if shooting self
            bool notSelf = !(targetX == CurrentRobotsX && targetY == CurrentRobotsY);

            // check if surrounding 8 blocks
            bool within8Blocks = (dx <= 1 && dy <= 1); 

            // check whether in bounds
            bool insideMap = (targetX >= 0 && targetX < battlefieldWidth && targetY >= 0 && targetY < battlefieldHeight);

            validTarget = notSelf && within8Blocks && insideMap;

            if (!validTarget) {
                cout << "Invalid target. Please choose a tile next to you, not yourself, and within the map"<<endl;
            }

        } while (!validTarget);

        int ShellLeft = this->numOfShell();
        if (ShellLeft == 0) {
            cout<<"Oh no! You ran out of shells! You can't shoot anything!"<<endl;
            return;
        }


        bool hit = false;

        for (Robot* robot : battlefield->robots()) { 
            string targetRobotId = robot->id() ;
            int PotentialRobotX = robot->x() ;
            int PotentialRobotY = robot->y() ;
            if (targetX == CurrentRobotsX && targetY == CurrentRobotsY ){
                cout<<"\nYou can't Shoot Yourself"<<endl;
                break;
            }
            if (targetX == PotentialRobotX && targetY == PotentialRobotY ){
                int hitChance = rand() % 100; // number from 0-99
                if (hitChance < 70) { // 70% chance to hit
                    cout<<"\nYou've successfully shot an enemy Robot!"<<endl;
                    robot->reduceLives();

                    if (!robot->isAlive()) {
                        cout << targetRobotId << " has been destroyed!" << endl;

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

                    // cout << "Remaining Robots: " << endl;
                    // for (const auto& r : battlefield->robots()) {
                    //     cout << *r << endl;
                    // }


                    if (this->canUpgrade()) {
                        this->incrementUpgradeCount();

                        cout<<"\n" << this->id() <<" earned an upgrade! A random upgrade is applied!" << endl;

                        // Randomly choose upgrade category (1 - Moving, 2 - Shooting, 3 - Extra)
                        int choice = rand() % 3 + 1;

                        switch (choice) {
                            case 1: {
                                // Randomly choose Moving upgrade (1 - HideBot, 2 - JumpBot)
                                int moveChoice = rand() % 2 + 1;
                                if (moveChoice == 1) {
                                    cout <<this->id() <<" are now upgraded to HideBot!"<< endl;
                                    this->setRobotType("HideBot");
                                } else {
                                    cout <<this->id() <<" are now upgraded to JumpBot!"<< endl;
                                    this->setRobotType("JumpBot");
                                }
                                break;
                            }
                            case 2: {
                                // Randomly choose Shooting upgrade (1 - LongShotBot, 2 - SemiAutoBot, 3 - ThirtyShotBot)
                                int shootChoice = rand() % 3 + 1;
                                if (shootChoice == 1) {
                                    cout <<this->id() <<" are now upgraded to LongShotBot!"<< endl;
                                    this->setRobotType("LongShotBot");
                                } else if (shootChoice == 2) {
                                    cout <<this->id() <<" are now upgraded to SemiAutoBot!"<< endl;
                                    this->setRobotType("ScoutBot");
                                } else {
                                    cout <<this->id() <<" are now upgraded to ThirtyShotBot!"<< endl;
                                    this->setRobotType("ThirtyShotBot");
                                    //ThirtyShotBot(this->id().substr(5),this->x(),this->y());
                                }
                                break;
                            }
                            // case 3: {
                            //     // Randomly choose Seeing upgrade (1 - ScoutBot, 2 - TrackBot)
                            //     int seeChoice = rand() % 2 + 1;
                            //     if (seeChoice == 1) {
                            //         cout <<this->id() <<" are now upgraded to ScoutBot!"<< endl;
                            //         this->setRobotType("ScoutBot");
                            //     } else {
                            //         cout <<this->id() <<" are now upgraded to TrackBot!"<< endl;
                            //         this->setRobotType("TrackBot");
                            //     }
                            //     break;
                            // }
                            case 3: {
                                int extraChoice = rand() % 3 + 1;
                                if (extraChoice == 1) {
                                    cout << this->id() << " are now upgraded to NukeBot!" << endl;
                                    this->setRobotType("NukeBot");
                                } else if (extraChoice == 2) {
                                    cout << this->id() << " are now upgraded to GodBot!" << endl;
                                    this->setRobotType("GodBot");
                                } else {
                                    cout << this->id() << " are now upgraded to JukeBot!" << endl;
                                    this->setRobotType("JukeBot");
                                }
                                break;
                            }
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

    void actions(Battlefield* battlefield) override {
        int choice;

        cout << "ScoutBot Actions:" << endl;
        cout << "1. ScoutBot actionMove" << endl;
        cout << "2. ScoutBot actionShoot" << endl;
        cout << "3. ScoutBot actionLook" << endl;
        cout << "4. ScoutBot actionThink" << endl;
        cout << "5. ScoutBot actionSkip" << endl;

        cout << "Please choose your action: ";
        cin >> choice;

        switch (choice) {
            case 1:
                actionMove(battlefield);
                break;
            case 2:
                actionShoot(battlefield);
                break;
            case 3:
                actionLook(battlefield);
                break;
            case 4:
                actionThink(battlefield);
                break;
            case 5:
                cout << "Skipping actions." << endl;
                break;
            default:
                cout << "Invalid choice. Please try again." << endl;
        }
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
        id_ = id;
        robotType_ = "ScoutBot";
        robotPosX = x;
        robotPosY = y;
        robotName_ = "SB" + id;
    }

    virtual ~TrackBot() {}

    void setLocations(int x, int y) override {
        robotPosX = x;
        robotPosY = y;
    }

    void actionThink(Battlefield* battlefield) override {
        cout << "HideBot is analyzing the battlefield..." << endl;

        // Track nearby robots
        if (trackedRobots_.empty()) {
            cout << "No robots are being tracked." << endl;
        } else {
            cout << "Tracked Robots:" << endl;
            for (const auto& tracked : trackedRobots_) {
                Robot* robot = tracked;
                cout << " - " << robot->id() << " at (" << robot->x() << ", " << robot->y() << ")" << endl;
            }
        }
    }

    void actionLook(Battlefield* battlefield) override {
        cout << "=== TrackBot View (Tracked Targets) ===" << endl;

                // Display tracked robots
        if (trackedRobots_.empty()) {
            cout << "No active trackers. Plant trackers to see enemy locations." << endl;
        } else {
            // Display column headers
            cout << "     ";
            for (int x = 0; x < battlefield->BATTLEFIELD_NUM_OF_COLS(); ++x) {
                cout << "  " << right << setfill('0') << setw(2) << x << " ";
            }
            cout << endl;

            for (int y = 0; y < battlefield->BATTLEFIELD_NUM_OF_ROWS(); ++y) {
                // Display row border
                cout << "   ";
                for (int x = 0; x < battlefield->BATTLEFIELD_NUM_OF_COLS(); ++x) {
                    cout << "+----";
                }
                cout << "+" << endl;

                // Display row number
                cout << " " << right << setfill('0') << setw(2) << y << " ";
                
                // Display row content
                for (int x = 0; x < battlefield->BATTLEFIELD_NUM_OF_COLS(); ++x) {
                    bool isTracked = false;
                    string trackedId;
                    
                    // Check if this cell has a tracked robot
                    for (const auto& tracked : trackedRobots_) {
                        Robot* robot = tracked;
                        if (robot->x() == x && robot->y() == y) {
                            isTracked = true;
                            trackedId = robot->id();
                            break;
                        }
                    }

                    if (x == robotPosX && y == robotPosY) {
                        cout << "|" << left << setfill(' ') << setw(4) << "[R]"; // Mark robot itself
                    } else if (isTracked) {
                        cout << "|" << left << setfill(' ') << setw(4) << trackedId.substr(0, 3); // Show tracked robot
                    } else {
                        string content = battlefield->getCellContent(x, y);
                        if (content.empty()) {
                            cout << "|    ";
                        } else {
                            cout << "|" << left << setfill(' ') << setw(4) << "?"; // Unknown robots
                        }
                    }
                }
                cout << "|" << endl;
            }

            // Display bottom border
            cout << "   ";
            for (int x = 0; x < battlefield->BATTLEFIELD_NUM_OF_COLS(); ++x) {
                cout << "+----";
            }
            cout << "+" << endl;
        }
    }

    void actionMove(Battlefield* battlefield) override {
        int currentX = robotPosX;
        int currentY = robotPosY;

        vector<string> directions = {"UL", "U", "UR", "L", "C", "R", "DL", "D", "DR"};
        vector<pair<int, int>> directionsMove = {
            {-1, -1}, {0, -1}, {1, -1},
            {-1, 0}, {0, 0}, {1, 0},
            {-1, 1}, {0, 1}, {1, 1}
        };

        // Show available directions
        cout << "\nAvailable Directions:\n";
        cout << "UL, U, UR\n";
        cout << "L, C, R\n";
        cout << "DL, D, DR\n";

        // Enter direction input
        string userInput;
        cout << "Enter direction: ";
        cin >> userInput;

        // Find matching direction
        int moveIndex = -1;
        for (int i = 0; i < directions.size(); i++) {
            if (directions[i] == userInput) {
                moveIndex = i;
                break;
            }
        }

        // Check invalid input 
        if (moveIndex == -1) {
            cout << "Invalid move direction\n";
            return;
        }

        // Calculate new position
        int newX = currentX;
        int newY = currentY;

        if (userInput != "C") {
            newX += directionsMove[moveIndex].first;
            newY += directionsMove[moveIndex].second;
        }

        // Check if moves within the boundary
        if (newX < 0 || newX >= battlefield->BATTLEFIELD_NUM_OF_COLS() ||
            newY < 0 || newY >= battlefield->BATTLEFIELD_NUM_OF_ROWS()) {
            cout << "Cannot move outside battlefield\n";
            return;
        }

        // Check if destination is occupied
        for (Robot* robot : battlefield->robots()) { 
            int PotentialRobotX = robot->x();
            int PotentialRobotY = robot->y();

            if (newX == PotentialRobotX && newY == PotentialRobotY) {
                cout << "Destination occupied" << endl;
                return;  // Stop and do not move if the destination is occupied
            }
        }

        // If we reach here, it means the destination is free
        robotPosX = newX;
        robotPosY = newY;

        cout << "Moved to (" << newX << "," << newY << ")" << endl;
    }

    void actionShoot(Battlefield* battlefield) override {
        string CurrentRobotsName = this->robotName();
        cout<<"The Current Robot controlled is "<<CurrentRobotsName<<endl;

        int CurrentRobotsX = this->x();
        int CurrentRobotsY = this->y();

        int targetX, targetY;

        bool validTarget = false;

        int battlefieldWidth = battlefield->BATTLEFIELD_NUM_OF_COLS();    
        int battlefieldHeight = battlefield->BATTLEFIELD_NUM_OF_ROWS();  

        do {
            cout << "Enter your target coordinates (X Y): ";
            cin >> targetX >> targetY;

            // calculate distance between target and self
            int dx = abs(targetX - CurrentRobotsX);
            int dy = abs(targetY - CurrentRobotsY);

            // check if shooting self
            bool notSelf = !(targetX == CurrentRobotsX && targetY == CurrentRobotsY);

            // check if surrounding 8 blocks
            bool within8Blocks = (dx <= 1 && dy <= 1); 

            // check whether in bounds
            bool insideMap = (targetX >= 0 && targetX < battlefieldWidth && targetY >= 0 && targetY < battlefieldHeight);

            validTarget = notSelf && within8Blocks && insideMap;

            if (!validTarget) {
                cout << "Invalid target. Please choose a tile next to you, not yourself, and within the map"<<endl;
            }

        } while (!validTarget);

        int ShellLeft = this->numOfShell();
        if (ShellLeft == 0) {
            cout<<"Oh no! You ran out of shells! You can't shoot anything!"<<endl;
            return;
        }


        bool hit = false;

        for (Robot* robot : battlefield->robots()) { 
            string targetRobotId = robot->id() ;
            int PotentialRobotX = robot->x() ;
            int PotentialRobotY = robot->y() ;
            if (targetX == CurrentRobotsX && targetY == CurrentRobotsY ){
                cout<<"\nYou can't Shoot Yourself"<<endl;
                break;
            }
            if (targetX == PotentialRobotX && targetY == PotentialRobotY ){
                int hitChance = rand() % 100; // number from 0-99
                if (hitChance < 70) { // 70% chance to hit
                    cout<<"\nYou've successfully shot an enemy Robot!"<<endl;
                    robot->reduceLives();

                    if (!robot->isAlive()) {
                        cout << targetRobotId << " has been destroyed!" << endl;

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

                    // cout << "Remaining Robots: " << endl;
                    // for (const auto& r : battlefield->robots()) {
                    //     cout << *r << endl;
                    // }


                    if (this->canUpgrade()) {
                        this->incrementUpgradeCount();

                        cout<<"\n" << this->id() <<" earned an upgrade! A random upgrade is applied!" << endl;
                        cout <<this->id() <<" are now upgraded to ScoutBot!"<< endl;
                        ScoutBot* upgradedRobot = new ScoutBot(this->id().substr(5), this->x(), this->y());
                        // IM LOSING MY SHIT HERE

                        //Randomly choose upgrade category (1 - Moving, 2 - Shooting, 3 - Extra)
                        int choice = rand() % 3 + 1;

                        switch (choice) {
                            case 1: {
                                // Randomly choose Moving upgrade (1 - HideBot, 2 - JumpBot)
                                int moveChoice = rand() % 2 + 1;
                                if (moveChoice == 1) {
                                    cout <<this->id() <<" are now upgraded to HideBot!"<< endl;
                                    this->setRobotType("HideBot");
                                } else if (moveChoice == 2) {
                                    cout <<this->id() <<" are now upgraded to JumpBot!"<< endl;
                                    this->setRobotType("JumpBot");
                                } else {
                                    cout <<this->id() <<" are now upgraded to JukeBot!"<< endl;
                                    this->setRobotType("JukeBot");
                                }
                                break;
                            }
                            case 2: {
                                // Randomly choose Shooting upgrade (1 - LongShotBot, 2 - SemiAutoBot, 3 - ThirtyShotBot)
                                int shootChoice = rand() % 3 + 1;
                                if (shootChoice == 1) {
                                    cout <<this->id() <<" are now upgraded to LongShotBot!"<< endl;
                                    this->setRobotType("LongShotBot");
                                } else if (shootChoice == 2) {
                                    cout <<this->id() <<" are now upgraded to SemiAutoBot!"<< endl;
                                    this->setRobotType("ScoutBot");
                                } else {
                                    cout <<this->id() <<" are now upgraded to ThirtyShotBot!"<< endl;
                                    this->setRobotType("ThirtyShotBot");
                                    //ThirtyShotBot(this->id().substr(5),this->x(),this->y());
                                }
                                break;
                            }
                            // case 3: {
                            //     // Randomly choose Seeing upgrade (1 - ScoutBot, 2 - TrackBot)
                            //     int seeChoice = rand() % 2 + 1;
                            //     if (seeChoice == 1) {
                            //         cout <<this->id() <<" are now upgraded to ScoutBot!"<< endl;
                            //         this->setRobotType("ScoutBot");
                            //     } else {
                            //         cout <<this->id() <<" are now upgraded to TrackBot!"<< endl;
                            //         this->setRobotType("TrackBot");
                            //     }
                            //     break;
                            // }
                            case 3: {
                                int extraChoice = rand() % 2 + 1;
                                if (extraChoice == 1) {
                                    cout << this->id() << " are now upgraded to NukeBot!" << endl;
                                    this->setRobotType("NukeBot");
                                } else if (extraChoice == 2) {
                                    cout << this->id() << " are now upgraded to GodBot!" << endl;
                                    this->setRobotType("GodBot");
                                }
                                break;
                            }
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

    void plantTracker(Battlefield* battlefield) {
        if (trackedRobots_.size() >= MAX_TRACKERS) {
            cout << "You can only track " << MAX_TRACKERS << " robots at a time!" << endl;
            return;
        }

        int targetX, targetY;
        cout << "Enter coordinates to plant tracker (X Y): ";
        cin >> targetX >> targetY;

        // Check if target is in surrounding cells
        int dx = abs(targetX - robotPosX);
        int dy = abs(targetY - robotPosY);
        if (dx > 1 || dy > 1) {
            cout << "Can only plant trackers on surrounding squares!" << endl;
            return;
        }

        // Check if target is valid
        if (targetX < 0 || targetX >= battlefield->BATTLEFIELD_NUM_OF_COLS() ||
            targetY < 0 || targetY >= battlefield->BATTLEFIELD_NUM_OF_ROWS()) {
            cout << "Invalid coordinates!" << endl;
            return;
        }

        // Find robot at target location
        Robot* targetRobot = nullptr;
        for (Robot* robot : battlefield->robots()) {
            if (robot->x() == targetX && robot->y() == targetY && robot != this) {
                targetRobot = robot;
                break;
            }
        }

        if (targetRobot == nullptr) {
            cout << "No enemy robot at target location!" << endl;
            return;
        }

        // Check if already tracking this robot
        for (auto& tracked : trackedRobots_) {
            if (tracked == targetRobot) {
                cout << "Already tracking this robot!" << endl;
                return;
            }
        }

        // Plant the tracker
        trackedRobots_.emplace_back(targetRobot);
        cout << "Tracker planted on " << targetRobot->id() << " at (" << targetRobot->x() << ", " << targetRobot->y() << ")." << endl;
    }

    void actions(Battlefield* battlefield) override {
        int choice;

        cout << "TrackBot Actions:" << endl;
        cout << "1. TrackBot actionMove" << endl;
        cout << "2. TrackBot actionShoot" << endl;
        cout << "3. TrackBot actionLook" << endl;
        cout << "4. TrackBot actionThink" << endl;
        cout << "5. Plant Tracker" << endl;

        cout << "Please choose your action: ";
        cin >> choice;

        switch (choice) {
            case 1:
                actionMove(battlefield);
                break;
            case 2:
                actionShoot(battlefield);
                break;
            case 3:
                actionLook(battlefield);
                break;
            case 4:
                actionThink(battlefield);
                break;
            case 5:
                plantTracker(battlefield);
                break;
            default:
                cout << "Invalid choice. Please try again." << endl;
        }
    }
};



// Shooting Upgrades
class LongShotBot: public ThinkingRobot, public SeeingRobot, 
                public ShootingRobot, public MovingRobot 
{
public:
    LongShotBot(string id = "", int x = -1, int y = -1) {
        id_ = id;
        robotType_ = "LongShotBot";
        robotPosX = x;
        robotPosY = y;
        robotName_ = "LSB" + id;
    }

    virtual ~LongShotBot() {}

    void setLocations(int x, int y) override {
        robotPosX = x;
        robotPosY = y;
    }

    void actionThink(Battlefield* battlefield) override {
        cout << "LongShotBot is analyzing long-range targets..." << endl;
    }

    void actionLook(Battlefield* battlefield) override {
        
        int currentX = robotPosX;
        int currentY = robotPosY;

        cout << "=== Robot View (3x3 Grid) ===" << endl;

        for (int y = currentY - 1; y <= currentY + 1; ++y) {
            for (int x = currentX - 1; x <= currentX + 1; ++x) {
                // Check bounds
                if (y >= 0 && y < battlefield->BATTLEFIELD_NUM_OF_ROWS() &&
                    x >= 0 && x < battlefield->BATTLEFIELD_NUM_OF_COLS()) {

                    if (x == currentX && y == currentY) {
                        cout << "[R] ";  // Mark robot itself
                    } else {
                        string content = battlefield->getCellContent(x, y); // helper call
                        if (content.empty()) {
                            cout << "[ ] ";
                        } else {
                            cout << "["<< content.substr(0,1) <<"] "; // Show first letter or symbol
                        }
                    }

                } else {
                    cout << "[#] "; // Out of bounds
                }
            }
            cout << endl;
        }
    }

    void actionMove(Battlefield* battlefield) {
        int currentX = robotPosX;
        int currentY = robotPosY;

        vector<string> directions = {"UL", "U", "UR", "L", "C", "R", "DL", "D", "DR"};
        vector<pair<int, int>> directionsMove = {
            {-1, -1}, {0, -1}, {1, -1},
            {-1, 0}, {0, 0}, {1, 0},
            {-1, 1}, {0, 1}, {1, 1}
        };

        // Show available directions
        cout << "\nAvailable Directions:\n";
        cout << "UL, U, UR\n";
        cout << "L, C, R\n";
        cout << "DL, D, DR\n";

        // Enter direction input
        string userInput;
        cout << "Enter direction: ";
        cin >> userInput;

        // Find matching direction
        int moveIndex = -1;
        for (int i = 0; i < directions.size(); i++) {
            if (directions[i] == userInput) {
                moveIndex = i;
                break;
            }
        }

        // Check invalid input 
        if (moveIndex == -1) {
            cout << "Invalid move direction\n";
            return;
        }

        // Calculate new position
        int newX = currentX;
        int newY = currentY;

        if (userInput != "C") {
            newX += directionsMove[moveIndex].first;
            newY += directionsMove[moveIndex].second;
        }

        // Check if moves within the boundary
        if (newX < 0 || newX >= battlefield->BATTLEFIELD_NUM_OF_COLS() ||
            newY < 0 || newY >= battlefield->BATTLEFIELD_NUM_OF_ROWS()) {
            cout << "Cannot move outside battlefield\n";
            return;
        }

        // Check if destination is occupied
        for (Robot* robot : battlefield->robots()) { 
            int PotentialRobotX = robot->x();
            int PotentialRobotY = robot->y();

            if (newX == PotentialRobotX && newY == PotentialRobotY) {
                cout << "Destination occupied" << endl;
                return;  // Stop and do not move if the destination is occupied
            }
        }

        // If we reach here, it means the destination is free
        robotPosX = newX;
        robotPosY = newY;

        cout << "Moved to (" << newX << "," << newY << ")" << endl;
    }

    void actionShoot (Battlefield* battlefield) override {
        // Implement the logic for shooting robot actions here
        cout << "GenericRobot actionShoot" << endl;

        string CurrentRobotsName = this->robotName();
        cout<<"The Current Robot controlled is "<<CurrentRobotsName<<endl;

        int CurrentRobotsX = this->x();
        int CurrentRobotsY = this->y();

        int targetX, targetY;

        bool validTarget = false;

        int battlefieldWidth = battlefield->BATTLEFIELD_NUM_OF_COLS();    
        int battlefieldHeight = battlefield->BATTLEFIELD_NUM_OF_ROWS();  

        do {
            cout << "Enter your target coordinates (X Y): ";
            cin >> targetX >> targetY;

            int dx = abs(targetX - CurrentRobotsX);
            int dy = abs(targetY - CurrentRobotsY);
            int distance = dx + dy;

            // check if shooting self
            bool notSelf = !(targetX == CurrentRobotsX && targetY == CurrentRobotsY);

            // check if surrounding 8 blocks
            bool withinRange = (distance <= 3);  

            // check whether in bounds
            bool insideMap = (targetX >= 0 && targetX < battlefieldWidth && targetY >= 0 && targetY < battlefieldHeight);

            validTarget = notSelf && withinRange && insideMap;

            if (!validTarget) {
                cout << "Invalid target. Please choose a tile next to you, not yourself, and within the map"<<endl;
            }

        } while (!validTarget);

        int ShellLeft = this->numOfShell();
        if (ShellLeft == 0) {
            cout<<"Oh no! You ran out of shells! You can't shoot anything!"<<endl;
            return;
        }


        bool hit = false;

        for (Robot* robot : battlefield->robots()) { 
            string targetRobotId = robot->id() ;
            int PotentialRobotX = robot->x() ;
            int PotentialRobotY = robot->y() ;
            if (targetX == CurrentRobotsX && targetY == CurrentRobotsY ){
                cout<<"\nYou can't Shoot Yourself"<<endl;
                break;
            }
            if (targetX == PotentialRobotX && targetY == PotentialRobotY ){
                int hitChance = rand() % 100; // number from 0-99
                if (hitChance < 70) { // 70% chance to hit
                    cout<<"\nYou've successfully shot an enemy Robot!"<<endl;
                    robot->reduceLives();

                    if (!robot->isAlive()) {
                        cout << targetRobotId << " has been destroyed!" << endl;

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

                    // cout << "Remaining Robots: " << endl;
                    // for (const auto& r : battlefield->robots()) {
                    //     cout << *r << endl;
                    // }


                    if (this->canUpgrade()) {
                        this->incrementUpgradeCount();

                        cout<<"\n" << this->id() <<" earned an upgrade! A random upgrade is applied!" << endl;
                        cout <<this->id() <<" are now upgraded to ScoutBot!"<< endl;
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
                                    this->setRobotType("HideBot");
                                } else if (moveChoice == 2) {
                                    cout <<this->id() <<" are now upgraded to JumpBot!"<< endl;
                                    this->setRobotType("JumpBot");
                                } else {
                                    cout <<this->id() <<" are now upgraded to JukeBot!"<< endl;
                                    this->setRobotType("JukeBot");
                                }
                                break;
                            }
                            // case 2: {
                            //     // Randomly choose Shooting upgrade (1 - LongShotBot, 2 - SemiAutoBot, 3 - ThirtyShotBot)
                            //     int shootChoice = rand() % 3 + 1;
                            //     if (shootChoice == 1) {
                            //         cout <<this->id() <<" are now upgraded to LongShotBot!"<< endl;
                            //         this->setRobotType("LongShotBot");
                            //     } else if (shootChoice == 2) {
                            //         cout <<this->id() <<" are now upgraded to SemiAutoBot!"<< endl;
                            //         this->setRobotType("ScoutBot");
                            //     } else {
                            //         cout <<this->id() <<" are now upgraded to ThirtyShotBot!"<< endl;
                            //         this->setRobotType("ThirtyShotBot");
                            //         //ThirtyShotBot(this->id().substr(5),this->x(),this->y());
                            //     }
                            //     break;
                            // }
                            case 2: {
                                // Randomly choose Seeing upgrade (1 - ScoutBot, 2 - TrackBot)
                                int seeChoice = rand() % 2 + 1;
                                if (seeChoice == 1) {
                                    cout <<this->id() <<" are now upgraded to ScoutBot!"<< endl;
                                    this->setRobotType("ScoutBot");
                                } else {
                                    cout <<this->id() <<" are now upgraded to TrackBot!"<< endl;
                                    this->setRobotType("TrackBot");
                                }
                                break;
                            }
                            case 3: {
                                int extraChoice = rand() % 2 + 1;
                                if (extraChoice == 1) {
                                    cout << this->id() << " are now upgraded to NukeBot!" << endl;
                                    this->setRobotType("NukeBot");
                                } else if (extraChoice == 2) {
                                    cout << this->id() << " are now upgraded to GodBot!" << endl;
                                    this->setRobotType("GodBot");
                                }
                                break;
                            }
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

    void actions(Battlefield* battlefield) override {
        int choice;

        cout << "LongShotBot Actions:" << endl;
        cout << "1. LongShotBot actionMove" << endl;
        cout << "2. LongShotBot actionShoot" << endl;
        cout << "3. LongShotBot actionLook" << endl;
        cout << "4. LongShotBot actionThink" << endl;

        cout << "Please choose your action: ";
        cin >> choice;

        switch (choice) {
            case 1:
                actionMove(battlefield);
                break;
            case 2:
                actionShoot(battlefield);
                break;
            case 3:
                actionLook(battlefield);
                break;
            case 4:
                actionThink(battlefield);
                break;
            default:
                cout << "Invalid choice. Please try again." << endl;
        }
    }
};

class SemiAutoBot: public ThinkingRobot, public SeeingRobot, 
                public ShootingRobot, public MovingRobot 
{
public:
    SemiAutoBot(string id = "", int x = -1, int y = -1) {
        id_ = id;
        robotType_ = "SemiAutoBot";
        robotPosX = x;
        robotPosY = y;
        robotName_ = "SAB" + id;
    }

    virtual ~SemiAutoBot() {}

    void setLocations(int x, int y) override {
        robotPosX = x;
        robotPosY = y;
    }

    void actionThink(Battlefield* battlefield) override {
        cout << "LongShotBot is analyzing long-range targets..." << endl;
    }

    void actionLook(Battlefield* battlefield) override {
        
        int currentX = robotPosX;
        int currentY = robotPosY;

        cout << "=== Robot View (3x3 Grid) ===" << endl;

        for (int y = currentY - 1; y <= currentY + 1; ++y) {
            for (int x = currentX - 1; x <= currentX + 1; ++x) {
                // Check bounds
                if (y >= 0 && y < battlefield->BATTLEFIELD_NUM_OF_ROWS() &&
                    x >= 0 && x < battlefield->BATTLEFIELD_NUM_OF_COLS()) {

                    if (x == currentX && y == currentY) {
                        cout << "[R] ";  // Mark robot itself
                    } else {
                        string content = battlefield->getCellContent(x, y); // helper call
                        if (content.empty()) {
                            cout << "[ ] ";
                        } else {
                            cout << "["<< content.substr(0,1) <<"] "; // Show first letter or symbol
                        }
                    }

                } else {
                    cout << "[#] "; // Out of bounds
                }
            }
            cout << endl;
        }
    }

    void actionMove(Battlefield* battlefield) {
        int currentX = robotPosX;
        int currentY = robotPosY;

        vector<string> directions = {"UL", "U", "UR", "L", "C", "R", "DL", "D", "DR"};
        vector<pair<int, int>> directionsMove = {
            {-1, -1}, {0, -1}, {1, -1},
            {-1, 0}, {0, 0}, {1, 0},
            {-1, 1}, {0, 1}, {1, 1}
        };

        // Show available directions
        cout << "\nAvailable Directions:\n";
        cout << "UL, U, UR\n";
        cout << "L, C, R\n";
        cout << "DL, D, DR\n";

        // Enter direction input
        string userInput;
        cout << "Enter direction: ";
        cin >> userInput;

        // Find matching direction
        int moveIndex = -1;
        for (int i = 0; i < directions.size(); i++) {
            if (directions[i] == userInput) {
                moveIndex = i;
                break;
            }
        }

        // Check invalid input 
        if (moveIndex == -1) {
            cout << "Invalid move direction\n";
            return;
        }

        // Calculate new position
        int newX = currentX;
        int newY = currentY;

        if (userInput != "C") {
            newX += directionsMove[moveIndex].first;
            newY += directionsMove[moveIndex].second;
        }

        // Check if moves within the boundary
        if (newX < 0 || newX >= battlefield->BATTLEFIELD_NUM_OF_COLS() ||
            newY < 0 || newY >= battlefield->BATTLEFIELD_NUM_OF_ROWS()) {
            cout << "Cannot move outside battlefield\n";
            return;
        }

        // Check if destination is occupied
        for (Robot* robot : battlefield->robots()) { 
            int PotentialRobotX = robot->x();
            int PotentialRobotY = robot->y();

            if (newX == PotentialRobotX && newY == PotentialRobotY) {
                cout << "Destination occupied" << endl;
                return;  // Stop and do not move if the destination is occupied
            }
        }

        // If we reach here, it means the destination is free
        robotPosX = newX;
        robotPosY = newY;

        cout << "Moved to (" << newX << "," << newY << ")" << endl;
    }

    void actionShoot(Battlefield* battlefield) override {
        string CurrentRobotsName = this->robotName();
        cout << "The Current Robot controlled is " << CurrentRobotsName << endl;

        int CurrentRobotsX = this->x();
        int CurrentRobotsY = this->y();

        int targetX, targetY;

        bool validTarget = false;

        int battlefieldWidth = battlefield->BATTLEFIELD_NUM_OF_COLS();    
        int battlefieldHeight = battlefield->BATTLEFIELD_NUM_OF_ROWS();  

        do {
            cout << "Enter your target coordinates (X Y): ";
            cin >> targetX >> targetY;

            // calculate distance between target and self
            int dx = abs(targetX - CurrentRobotsX);
            int dy = abs(targetY - CurrentRobotsY);

            // check if shooting self
            bool notSelf = !(targetX == CurrentRobotsX && targetY == CurrentRobotsY);

            // check if surrounding 8 blocks
            bool within8Blocks = (dx <= 1 && dy <= 1); 

            // check whether in bounds
            bool insideMap = (targetX >= 0 && targetX < battlefieldWidth && targetY >= 0 && targetY < battlefieldHeight);

            validTarget = notSelf && within8Blocks && insideMap;

            if (!validTarget) {
                cout << "Invalid target. Please choose a tile next to you, not yourself, and within the map" << endl;
            }

        } while (!validTarget);

        int ShellLeft = this->numOfShell();
        if (ShellLeft < 3) {  // Need 3 shells for a burst
            cout << "Oh no! You need at least 3 shells for a burst shot!" << endl;
            return;
        }

        bool hit = false;

        for (int i = 0; i < 3; ++i) { 
            for (Robot* robot : battlefield->robots()) { 
                string targetRobotId = robot->id() ;
                int PotentialRobotX = robot->x() ;
                int PotentialRobotY = robot->y() ;
                if (targetX == CurrentRobotsX && targetY == CurrentRobotsY ){
                    cout<<"\nYou can't Shoot Yourself"<<endl;
                    break;
                }
                if (targetX == PotentialRobotX && targetY == PotentialRobotY ){
                    int hitChance = rand() % 100; // number from 0-99
                    if (hitChance < 70) { // 70% chance to hit
                        cout<<"\nYou've successfully shot an enemy Robot!"<<endl;
                        robot->reduceLives();

                        if (!robot->isAlive()) {
                            cout << targetRobotId << " has been destroyed!" << endl;

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

                        // cout << "Remaining Robots: " << endl;
                        // for (const auto& r : battlefield->robots()) {
                        //     cout << *r << endl;
                        // }


                        if (this->canUpgrade()) {
                            this->incrementUpgradeCount();

                            cout<<"\n" << this->id() <<" earned an upgrade! A random upgrade is applied!" << endl;
                            cout <<this->id() <<" are now upgraded to ScoutBot!"<< endl;
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
                                        this->setRobotType("HideBot");
                                    } else if (moveChoice == 2) {
                                        cout <<this->id() <<" are now upgraded to JumpBot!"<< endl;
                                        this->setRobotType("JumpBot");
                                    } else {
                                        cout <<this->id() <<" are now upgraded to JukeBot!"<< endl;
                                        this->setRobotType("JukeBot");
                                    }
                                    break;
                                }
                                // case 2: {
                                //     // Randomly choose Shooting upgrade (1 - LongShotBot, 2 - SemiAutoBot, 3 - ThirtyShotBot)
                                //     int shootChoice = rand() % 3 + 1;
                                //     if (shootChoice == 1) {
                                //         cout <<this->id() <<" are now upgraded to LongShotBot!"<< endl;
                                //         this->setRobotType("LongShotBot");
                                //     } else if (shootChoice == 2) {
                                //         cout <<this->id() <<" are now upgraded to SemiAutoBot!"<< endl;
                                //         this->setRobotType("ScoutBot");
                                //     } else {
                                //         cout <<this->id() <<" are now upgraded to ThirtyShotBot!"<< endl;
                                //         this->setRobotType("ThirtyShotBot");
                                //         //ThirtyShotBot(this->id().substr(5),this->x(),this->y());
                                //     }
                                //     break;
                                // }
                                case 2: {
                                    // Randomly choose Seeing upgrade (1 - ScoutBot, 2 - TrackBot)
                                    int seeChoice = rand() % 2 + 1;
                                    if (seeChoice == 1) {
                                        cout <<this->id() <<" are now upgraded to ScoutBot!"<< endl;
                                        this->setRobotType("ScoutBot");
                                    } else {
                                        cout <<this->id() <<" are now upgraded to TrackBot!"<< endl;
                                        this->setRobotType("TrackBot");
                                    }
                                    break;
                                }
                                case 3: {
                                    int extraChoice = rand() % 2 + 1;
                                    if (extraChoice == 1) {
                                        cout << this->id() << " are now upgraded to NukeBot!" << endl;
                                        this->setRobotType("NukeBot");
                                    } else if (extraChoice == 2) {
                                        cout << this->id() << " are now upgraded to GodBot!" << endl;
                                        this->setRobotType("GodBot");
                                    }
                                    break;
                                }
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
    }

    void actions(Battlefield* battlefield) override {
        int choice;

        cout << "SemiAutoBot Actions:" << endl;
        cout << "1. SemiAutoBot actionMove" << endl;
        cout << "2. SemiAutoBot actionShoot" << endl;
        cout << "3. SemiAutoBot actionLook" << endl;
        cout << "4. SemiAutoBot actionThink" << endl;

        cout << "Please choose your action: ";
        cin >> choice;

        switch (choice) {
            case 1:
                actionMove(battlefield);
                break;
            case 2:
                actionShoot(battlefield);
                break;
            case 3:
                actionLook(battlefield);
                break;
            case 4:
                actionThink(battlefield);
                break;
            default:
                cout << "Invalid choice. Please try again." << endl;
        }
    }
};

class ThirtyShotBot: public ThinkingRobot, public SeeingRobot, 
                public ShootingRobot, public MovingRobot 
{
public:
    ThirtyShotBot(string id = "", int x = -1, int y = -1) {
        id_ = id;
        robotType_ = "ThirtyShotBot";
        robotPosX = x;
        robotPosY = y;
        robotName_ = "TSB" + id;
        setNumOfShells(30);
    }

    virtual ~ThirtyShotBot() {}

    void setLocations(int x, int y) override {
        robotPosX = x;
        robotPosY = y;
    }

    void actionThink(Battlefield* battlefield) override {
        cout << "ThirtyShotBot is analyzing targets..." << endl;
    }

    void actionLook(Battlefield* battlefield) override {
                
        int currentX = robotPosX;
        int currentY = robotPosY;

        cout << "=== Robot View (3x3 Grid) ===" << endl;

        for (int y = currentY - 1; y <= currentY + 1; ++y) {
            for (int x = currentX - 1; x <= currentX + 1; ++x) {
                // Check bounds
                if (y >= 0 && y < battlefield->BATTLEFIELD_NUM_OF_ROWS() &&
                    x >= 0 && x < battlefield->BATTLEFIELD_NUM_OF_COLS()) {

                    if (x == currentX && y == currentY) {
                        cout << "[R] ";  // Mark robot itself
                    } else {
                        string content = battlefield->getCellContent(x, y); // helper call
                        if (content.empty()) {
                            cout << "[ ] ";
                        } else {
                            cout << "["<< content.substr(0,1) <<"] "; // Show first letter or symbol
                        }
                    }

                } else {
                    cout << "[#] "; // Out of bounds
                }
            }
            cout << endl;
        }
    }

    void actionMove(Battlefield* battlefield) {
        int currentX = robotPosX;
        int currentY = robotPosY;

        vector<string> directions = {"UL", "U", "UR", "L", "C", "R", "DL", "D", "DR"};
        vector<pair<int, int>> directionsMove = {
            {-1, -1}, {0, -1}, {1, -1},
            {-1, 0}, {0, 0}, {1, 0},
            {-1, 1}, {0, 1}, {1, 1}
        };

        // Show available directions
        cout << "\nAvailable Directions:\n";
        cout << "UL, U, UR\n";
        cout << "L, C, R\n";
        cout << "DL, D, DR\n";

        // Enter direction input
        string userInput;
        cout << "Enter direction: ";
        cin >> userInput;

        // Find matching direction
        int moveIndex = -1;
        for (int i = 0; i < directions.size(); i++) {
            if (directions[i] == userInput) {
                moveIndex = i;
                break;
            }
        }

        // Check invalid input 
        if (moveIndex == -1) {
            cout << "Invalid move direction\n";
            return;
        }

        // Calculate new position
        int newX = currentX;
        int newY = currentY;

        if (userInput != "C") {
            newX += directionsMove[moveIndex].first;
            newY += directionsMove[moveIndex].second;
        }

        // Check if moves within the boundary
        if (newX < 0 || newX >= battlefield->BATTLEFIELD_NUM_OF_COLS() ||
            newY < 0 || newY >= battlefield->BATTLEFIELD_NUM_OF_ROWS()) {
            cout << "Cannot move outside battlefield\n";
            return;
        }

        // Check if destination is occupied
        for (Robot* robot : battlefield->robots()) { 
            int PotentialRobotX = robot->x();
            int PotentialRobotY = robot->y();

            if (newX == PotentialRobotX && newY == PotentialRobotY) {
                cout << "Destination occupied" << endl;
                return;  // Stop and do not move if the destination is occupied
            }
        }
    }

    void actionShoot(Battlefield* battlefield) override {
        // Implement the logic for shooting robot actions here
        cout << "GenericRobot actionShoot" << endl;

        string CurrentRobotsName = this->robotName();
        cout<<"The Current Robot controlled is "<<CurrentRobotsName<<endl;

        int CurrentRobotsX = this->x();
        int CurrentRobotsY = this->y();

        int targetX, targetY;

        bool validTarget = false;

        int battlefieldWidth = battlefield->BATTLEFIELD_NUM_OF_COLS();    
        int battlefieldHeight = battlefield->BATTLEFIELD_NUM_OF_ROWS();  

        do {
            cout << "Enter your target coordinates (X Y): ";
            cin >> targetX >> targetY;

            // calculate distance between target and self
            int dx = abs(targetX - CurrentRobotsX);
            int dy = abs(targetY - CurrentRobotsY);

            // check if shooting self
            bool notSelf = !(targetX == CurrentRobotsX && targetY == CurrentRobotsY);

            // check if surrounding 8 blocks
            bool within8Blocks = (dx <= 1 && dy <= 1); 

            // check whether in bounds
            bool insideMap = (targetX >= 0 && targetX < battlefieldWidth && targetY >= 0 && targetY < battlefieldHeight);

            validTarget = notSelf && within8Blocks && insideMap;

            if (!validTarget) {
                cout << "Invalid target. Please choose a tile next to you, not yourself, and within the map"<<endl;
            }

        } while (!validTarget);

        int ShellLeft = this->numOfShell();
        if (ShellLeft == 0) {
            cout<<"Oh no! You ran out of shells! You can't shoot anything!"<<endl;
            return;
        }


        bool hit = false;

        for (Robot* robot : battlefield->robots()) { 
            string targetRobotId = robot->id() ;
            int PotentialRobotX = robot->x() ;
            int PotentialRobotY = robot->y() ;
            if (targetX == CurrentRobotsX && targetY == CurrentRobotsY ){
                cout<<"\nYou can't Shoot Yourself"<<endl;
                break;
            }
            if (targetX == PotentialRobotX && targetY == PotentialRobotY ){
                int hitChance = rand() % 100; // number from 0-99
                if (hitChance < 70) { // 70% chance to hit
                    cout<<"\nYou've successfully shot an enemy Robot!"<<endl;
                    robot->reduceLives();

                    if (!robot->isAlive()) {
                        cout << targetRobotId << " has been destroyed!" << endl;

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

                    // cout << "Remaining Robots: " << endl;
                    // for (const auto& r : battlefield->robots()) {
                    //     cout << *r << endl;
                    // }


                    if (this->canUpgrade()) {
                        this->incrementUpgradeCount();

                        cout<<"\n" << this->id() <<" earned an upgrade! A random upgrade is applied!" << endl;
                        cout <<this->id() <<" are now upgraded to ScoutBot!"<< endl;
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
                                    this->setRobotType("HideBot");
                                } else if (moveChoice == 2) {
                                    cout <<this->id() <<" are now upgraded to JumpBot!"<< endl;
                                    this->setRobotType("JumpBot");
                                } else {
                                    cout <<this->id() <<" are now upgraded to JukeBot!"<< endl;
                                    this->setRobotType("JukeBot");
                                }
                                break;
                            }
                            // case 2: {
                            //     // Randomly choose Shooting upgrade (1 - LongShotBot, 2 - SemiAutoBot, 3 - ThirtyShotBot)
                            //     int shootChoice = rand() % 3 + 1;
                            //     if (shootChoice == 1) {
                            //         cout <<this->id() <<" are now upgraded to LongShotBot!"<< endl;
                            //         this->setRobotType("LongShotBot");
                            //     } else if (shootChoice == 2) {
                            //         cout <<this->id() <<" are now upgraded to SemiAutoBot!"<< endl;
                            //         this->setRobotType("ScoutBot");
                            //     } else {
                            //         cout <<this->id() <<" are now upgraded to ThirtyShotBot!"<< endl;
                            //         this->setRobotType("ThirtyShotBot");
                            //         //ThirtyShotBot(this->id().substr(5),this->x(),this->y());
                            //     }
                            //     break;
                            // }
                            case 2: {
                                // Randomly choose Seeing upgrade (1 - ScoutBot, 2 - TrackBot)
                                int seeChoice = rand() % 2 + 1;
                                if (seeChoice == 1) {
                                    cout <<this->id() <<" are now upgraded to ScoutBot!"<< endl;
                                    this->setRobotType("ScoutBot");
                                } else {
                                    cout <<this->id() <<" are now upgraded to TrackBot!"<< endl;
                                    this->setRobotType("TrackBot");
                                }
                                break;
                            }
                            case 3: {
                                int extraChoice = rand() % 2 + 1;
                                if (extraChoice == 1) {
                                    cout << this->id() << " are now upgraded to NukeBot!" << endl;
                                    this->setRobotType("NukeBot");
                                } else if (extraChoice == 2) {
                                    cout << this->id() << " are now upgraded to GodBot!" << endl;
                                    this->setRobotType("GodBot");
                                }
                                break;
                            }
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
};



// Moving Upgrades

// Action Logics
/*
void GenericRobot::actionThink(Battlefield* battlefield) {
    // Implement the logic for thinking robot actions here
    cout << "GenericRobot actionThink" << endl;
}
*/

void GenericRobot::actionLook(Battlefield* battlefield) {
    
    int currentX = robotPosX;
    int currentY = robotPosY;

    cout << "=== Robot View (3x3 Grid) ===" << endl;

    for (int y = currentY - 1; y <= currentY + 1; ++y) {
        for (int x = currentX - 1; x <= currentX + 1; ++x) {
            // Check bounds
            if (y >= 0 && y < battlefield->BATTLEFIELD_NUM_OF_ROWS() &&
                x >= 0 && x < battlefield->BATTLEFIELD_NUM_OF_COLS()) {

                if (x == currentX && y == currentY) {
                    cout << "[R] ";  // Mark robot itself
                } else {
                    string content = battlefield->getCellContent(x, y); // helper call
                    if (content.empty()) {
                        cout << "[ ] ";
                    } else {
                        cout << "["<< content.substr(0,1) <<"] "; // Show first letter or symbol
                    }
                }

            } else {
                cout << "[#] "; // Out of bounds
            }
        }
        cout << endl;
    }
}

void GenericRobot::actionMove(Battlefield* battlefield) {
    int currentX = robotPosX;
    int currentY = robotPosY;

    vector<string> directions = {"UL", "U", "UR", "L", "C", "R", "DL", "D", "DR"};
    vector<pair<int, int>> directionsMove = {
        {-1, -1}, {0, -1}, {1, -1},
        {-1, 0}, {0, 0}, {1, 0},
        {-1, 1}, {0, 1}, {1, 1}
    };

    // Show available directions
    cout << "\nAvailable Directions:\n";
    cout << "UL, U, UR\n";
    cout << "L, C, R\n";
    cout << "DL, D, DR\n";

    // Enter direction input
    string userInput;
    cout << "Enter direction: ";
    cin >> userInput;

    // Find matching direction
    int moveIndex = -1;
    for (int i = 0; i < directions.size(); i++) {
        if (directions[i] == userInput) {
            moveIndex = i;
            break;
        }
    }

    // Check invalid input 
    if (moveIndex == -1) {
        cout << "Invalid move direction\n";
        return;
    }

    // Calculate new position
    int newX = currentX;
    int newY = currentY;

    if (userInput != "C") {
        newX += directionsMove[moveIndex].first;
        newY += directionsMove[moveIndex].second;
    }

    // Check if moves within the boundary
    if (newX < 0 || newX >= battlefield->BATTLEFIELD_NUM_OF_COLS() ||
        newY < 0 || newY >= battlefield->BATTLEFIELD_NUM_OF_ROWS()) {
        cout << "Cannot move outside battlefield\n";
        return;
    }

    // Check if destination is occupied
    for (Robot* robot : battlefield->robots()) { 
        int PotentialRobotX = robot->x();
        int PotentialRobotY = robot->y();

        if (newX == PotentialRobotX && newY == PotentialRobotY) {
            cout << "Destination occupied" << endl;
            return;  // Stop and do not move if the destination is occupied
        }
    }

    //If we reach here, it means the destination is free
    robotPosX = newX;
    robotPosY = newY;
    

    battlefield->setCell(currentX, currentY, nullptr);  // Clear the old cell
    battlefield->setCell(newX, newY, this);

    cout << "Moved to (" << newX << "," << newY << ")" << endl;
}  

void GenericRobot::actionShoot(Battlefield* battlefield) {
    // Implement the logic for shooting robot actions here
    cout << "GenericRobot actionShoot" << endl;

    string CurrentRobotsName = this->robotName();
    cout<<"The Current Robot controlled is "<<CurrentRobotsName<<endl;

    int CurrentRobotsX = this->x();
    int CurrentRobotsY = this->y();

    int targetX, targetY;

    bool validTarget = false;

    int battlefieldWidth = battlefield->BATTLEFIELD_NUM_OF_COLS();    
    int battlefieldHeight = battlefield->BATTLEFIELD_NUM_OF_ROWS();  

    do {
        cout << "Enter your target coordinates (X Y): ";
        cin >> targetX >> targetY;

        // calculate distance between target and self
        int dx = abs(targetX - CurrentRobotsX);
        int dy = abs(targetY - CurrentRobotsY);

        // check if shooting self
        bool notSelf = !(targetX == CurrentRobotsX && targetY == CurrentRobotsY);

        // check if surrounding 8 blocks
        bool within8Blocks = (dx <= 1 && dy <= 1); 

        // check whether in bounds
        bool insideMap = (targetX >= 0 && targetX < battlefieldWidth && targetY >= 0 && targetY < battlefieldHeight);

        validTarget = notSelf && within8Blocks && insideMap;

        if (!validTarget) {
            cout << "Invalid target. Please choose a tile next to you, not yourself, and within the map"<<endl;
        }

    } while (!validTarget);

    int ShellLeft = this->numOfShell();
    if (ShellLeft == 0) {
        cout<<"Oh no! You ran out of shells! You can't shoot anything!"<<endl;
        return;
    }


    bool hit = false;

    for (Robot* robot : battlefield->robots()) { 
        string targetRobotId = robot->id() ;
        int PotentialRobotX = robot->x() ;
        int PotentialRobotY = robot->y() ;
        if (targetX == CurrentRobotsX && targetY == CurrentRobotsY ){
            cout<<"\nYou can't Shoot Yourself"<<endl;
            break;
        }
        if (targetX == PotentialRobotX && targetY == PotentialRobotY ){
            int hitChance = rand() % 100; // number from 0-99
            if (hitChance < 70) { // 70% chance to hit
                cout<<"\nYou've successfully shot an enemy Robot!"<<endl;
                robot->reduceLives();

                if (!robot->isAlive()) {
                    cout << targetRobotId << " has been destroyed!" << endl;

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

                // cout << "Remaining Robots: " << endl;
                // for (const auto& r : battlefield->robots()) {
                //     cout << *r << endl;
                // }


                if (this->canUpgrade()) {
                    this->incrementUpgradeCount();

                    cout<<"\n" << this->id() <<" earned an upgrade! A random upgrade is applied!" << endl;
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
                                this->setRobotType("HideBot");
                            } else if (moveChoice == 2) {
                                cout <<this->id() <<" are now upgraded to JumpBot!"<< endl;
                                this->setRobotType("JumpBot");
                            } else {
                                cout <<this->id() <<" are now upgraded to JukeBot!"<< endl;
                                this->setRobotType("JukeBot");
                            }
                            break;
                        }
                        case 2: {
                            // Randomly choose Shooting upgrade (1 - LongShotBot, 2 - SemiAutoBot, 3 - ThirtyShotBot)
                            int shootChoice = rand() % 3 + 1;
                            if (shootChoice == 1) {
                                cout <<this->id() <<" are now upgraded to LongShotBot!"<< endl;
                                this->setRobotType("LongShotBot");
                            } else if (shootChoice == 2) {
                                cout <<this->id() <<" are now upgraded to SemiAutoBot!"<< endl;
                                this->setRobotType("ScoutBot");
                            } else {
                                cout <<this->id() <<" are now upgraded to ThirtyShotBot!"<< endl;
                                this->setRobotType("ThirtyShotBot");
                                //ThirtyShotBot(this->id().substr(5),this->x(),this->y());
                            }
                            break;
                        }
                        case 3: {
                            // Randomly choose Seeing upgrade (1 - ScoutBot, 2 - TrackBot)
                            int seeChoice = rand() % 2 + 1;
                            if (seeChoice == 1) {
                                cout <<this->id() <<" are now upgraded to ScoutBot!"<< endl;
                                this->setRobotType("ScoutBot");
                            } else {
                                cout <<this->id() <<" are now upgraded to TrackBot!"<< endl;
                                this->setRobotType("TrackBot");
                            }
                            break;
                        }
                        case 4: {
                            int extraChoice = rand() % 2 + 1;
                            if (extraChoice == 1) {
                                cout << this->id() << " are now upgraded to NukeBot!" << endl;
                                this->setRobotType("NukeBot");
                            } else if (extraChoice == 2) {
                                cout << this->id() << " are now upgraded to GodBot!" << endl;
                                this->setRobotType("GodBot");
                            }
                            break;
                        }
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




// Main Function
int main() {
    srand(time(0)); 

    Battlefield battlefield;

    // read the file
    battlefield.readFile("fileInput1.txt"); 
    
    // Get total number of turns and robot list
    int totalTurns = battlefield.turns();
    vector<Robot*> robots = battlefield.robots();
    int robotCount = robots.size();

    cout << "Total turns: " << totalTurns << endl;

    // Game Loop
    int currentTurn = 0;

    while (currentTurn < totalTurns) {
        // Determine which robot's turn it is
        int robotIndex = (currentTurn % robotCount);
        Robot* currentRobot = robots[robotIndex];

        // Skip dead robots
        if (!currentRobot->isAlive()) {
            currentTurn++;
            continue;
        }

        // Display the battlefield
        //battlefield.placeCurrentRobot(currentRobot);
        battlefield.placeRobots();
        //battlefield.placeCurrentRobot(currentRobot, currentTurn, robotCount);
        battlefield.displayBattlefield(currentRobot, currentTurn, robotCount);
        battlefield.updateWaitingRobots();

        

        cout << "Turn " << currentTurn + 1 << endl;

        if (currentRobot->x()== -1 && currentRobot->y()== -1){ // check if is in waiting queue
            cout<<"\nSkipping Turns, Robot "<<currentRobot->robotName()<<" is still in waiting queue.\n"<<endl;
            currentTurn++;
            continue;  
        }

        if (currentRobot->x()== -1 && currentRobot->y()== -2){ // check if is in destroyed queue
            cout<<"\nSkipping Turns, Robot "<<currentRobot->robotName()<<" is destroyed and out of the game!\n"<<endl;
            currentTurn++;
            continue;  
        }



        cout << "Robot Info: " << *currentRobot << endl;
        cout << "Robot Type: "<<currentRobot->robotType()<<endl;
        

        // should implement actions here
        currentRobot->actions(&battlefield);

        // char proceed;
        // cout << "Press 'y' to execute this robot's actions, or 'n' to quit: ";
        // cin >> proceed;

        // if (proceed == 'n' || proceed == 'N') {
        //     cout << "Exiting game loop." << endl;
        //     break;
        // } else if (proceed == 'y' || proceed == 'Y') {
        //     currentRobot->actions(&battlefield);  // Call the robot's action sequence
        // } else {
        //     cout << "Wrong input, skipping your turn." << endl;
        // }

        currentTurn++;
    }

    cout << "\nGame Over. Total turns played: " << currentTurn << endl;
    

    
    //Robot* robotGenericRobot = new GenericRobot("GR01", 4, 4);

    //cout << *robotGenericRobot << endl;
    //robotGenericRobot->actions(&battlefield);

    //delete robotGenericRobot;
    //robotGenericRobot = nullptr;

    return 0;
}