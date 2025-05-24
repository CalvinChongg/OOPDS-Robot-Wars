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

    string robotType() const { return robotType_; } // get robot type
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
        cout << "Thinking Robot " << id_ << " is thinking..." << endl;
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
    queue<Robot*> waitingRobots_;
    vector<vector<string>> battlefield_; //2D vector for battlefield

public:
    //Getter Functions
    int BATTLEFIELD_NUM_OF_COLS() const { return BATTLEFIELD_NUM_OF_COLS_; } // get number of columns
    int BATTLEFIELD_NUM_OF_ROWS() const { return BATTLEFIELD_NUM_OF_ROWS_; } // get number of rows
    int turns() const { return turns_; }
    int currentTurn() const { return turn; } // get current turn
    int numOfRobots() const { return numOfRobots_; }
    vector<Robot*> robots() const { return robots_; } // get robots
    bool isCellEmpty(int x, int y) const { return battlefield_[y][x].empty(); } // Check if the cell is empty
    
    string getCellContent(int x, int y) const {
        if (y >= 0 && y < BATTLEFIELD_NUM_OF_ROWS_ && x >= 0 && x < BATTLEFIELD_NUM_OF_COLS_) {
            return battlefield_[y][x];
        } else {
            return "";
        }
    }

    // Setter Functions
    void setCurrentTurn(int turn) { this->turn = turn; } // set current turn

    void setCell(int x, int y, Robot* robot) { 
        if (robot == nullptr) {
            battlefield_[y][x] = ""; // Clear the cell if robot is nullptr
        } else {
            battlefield_[y][x] = robot->id(); // Set the cell to the robot's id
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
                        cout << "[E] "; // Show first letter or symbol
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

    vector<pair<string, pair<int, int>>> directions = {
        {"TL", {-1, -1}}, {"T", {0, -1}}, {"TR", {1, -1}},
        {"L", {-1, 0}},   {"C", {0, 0}},  {"R", {1, 0}},
        {"BL", {-1, 1}},  {"B", {0, 1}},  {"BR", {1, 1}}
    };
    
    cout << "You can move to one of the 8 surrounding squares \n";

    // Ask for direction input
    string userInput;
    cout << "Enter direction (TL, T, TR, L, C, R, BL, B, BR): ";
    cin >> userInput;

    // Find the matching direction in the vector
    pair<int, int> delta = {0, 0};
    bool valid = false;
    for (const auto& dir : directions) {
        if (dir.first == userInput) {
            delta = dir.second;
            valid = true;
            break;
        }
    }

    if (!valid) {
        cout << "Invalid direction!" << endl;
        return;
    }

    int newX = currentX + delta.first;
    int newY = currentY + delta.second;

    // Check bounds
    if (newX < 0 || newX >= battlefield->BATTLEFIELD_NUM_OF_COLS() || newY < 0 || newY >= battlefield->BATTLEFIELD_NUM_OF_ROWS()) {
        cout << "Move out of bounds!" << endl;
        return;
    }

    // Check if destination is empty
    if (battlefield->isCellEmpty(newX, newY)) {
        // Move robot
        battlefield->setCell(newX, newY, this);
        battlefield->setCell(currentX, currentY, nullptr);

        // Update robot's stored position
        robotPosX = newX;
        robotPosY = newY;

        cout << "Moved to (" << newX << ", " << newY << ")" << endl;
    } else {
        cout << "Destination occupied!" << endl;
    }
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
                this->increaseKills();
                this->decreaseShell();
                int lifeLeft = robot->numOfLives();
                cout<< targetRobotId<<" now has "<<lifeLeft<<" of lives left"<<endl;
                cout<< this->id() <<" now has "<< this->numOfKills() <<" of kills!"<<endl;
                cout<< this->id() <<" now has "<< this->numOfShell() <<" of shells left!"<<endl;

                if (this->canUpgrade()) {
                    this->incrementUpgradeCount();

                    cout<<"\nYou've earned an upgrade! Choose one area to upgrade:"<<endl;
                    cout<<"REMINDER! You can't revert your chocies!"<<endl;
                    cout<<"1. Moving:     HideBot or JumpBot"<<endl;
                    cout<<"2. Shooting:   LongShotBot, SemiAutoBot or ThirtyShotBot"<<endl;
                    cout<<"3. Seeing:     ScoutBot or TrackBot"<<endl;

                    int choice;
                    do {
                        cout << "Enter 1, 2, or 3 for your upgrade category: ";
                        cin >> choice;
                    } while (choice < 1 || choice > 3);

                    switch (choice) {
                        case 1: {
                            int moveChoice;
                            cout << "Choose your Moving upgrade:\n";
                            cout << "1. HideBot "<<endl;
                            cout<<"2. JumpBot "<<endl;
                            cin >> moveChoice;
                            if (moveChoice == 1) {
                                cout<<"You are now upgraded to HideBot!"<<endl;
                                robot->setRobotType("HideBot");
                            } else {
                                cout<<"You are now upgraded to JumpBot!"<<endl;
                                robot->setRobotType("JumpBot");
                            }
                            break;
                        }
                        case 2: {
                            int shootChoice;
                            cout<<"Choose your Shooting upgrade: "<<endl;
                            cout<<"1. LongShotBot"<<endl;
                            cout<<"2. SemiAutoBot "<<endl;
                            cout<<"3. ThirtyShotBot "<<endl;
                            cin >> shootChoice;
                            if (shootChoice == 1) {
                                cout<<"You are now upgraded to LongShotBot!"<<endl;
                                robot->setRobotType("LongShotBot");
                            } else if (shootChoice == 2) {
                                cout<<"You are now upgraded to SemiAutoBot!"<<endl;
                                robot->setRobotType("SemiAutoBot");
                            } else {
                                cout<<"You are now upgraded to ThirtyShotBot!"<<endl;
                                robot->setRobotType("ThirtyShotBot");
                            }
                            break;
                        }
                        case 3: {
                            int seeChoice;
                            cout <<"Choose your Seeing upgrade: "<<endl;
                            cout <<"1. ScoutBot "<<endl;
                            cout<<"2. TrackBot "<<endl;
                            cin >> seeChoice;
                            if (seeChoice == 1) {
                                cout<<"You are now upgraded to ScoutBot!"<<endl;
                                robot->setRobotType("ScoutBot");
                            } else {
                                cout<<"You are now upgraded to TrackBot!"<<endl;
                                robot->setRobotType("TrackBot");
                            }
                            break;
                        }
                    }

                } else {
                    cout<<"Upgrade limit reached. You can only upgrade twice."<<endl;
                }
            } else {
                cout<<"Shot missed! The enemy robot was not hit."<<endl;
            }
            hit = true;
        }
    }

    if (!hit) {
        cout<<"No enemy robot was at the selected location."<<endl;
    }
}      

int main() {
    srand(1211109038);

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
        int robotIndex = currentTurn % robotCount;
        Robot* currentRobot = robots[robotIndex];

        // Skip dead robots
        if (!currentRobot->isAlive()) {
            currentTurn++;
            continue;
        }

        // Display the battlefield
        battlefield.placeRobots();
        battlefield.displayBattlefield();
        cout << "Turn " << currentTurn + 1 << endl;
        cout << "Robot Info: " << *currentRobot << endl;
        cout << "Robot Type: "<<currentRobot->robotType()<<endl;
        

        // should implement actions here
        char proceed;
        cout << "Press 'y' to execute this robot's actions, or 'n' to quit: ";
        cin >> proceed;

        if (proceed == 'n' || proceed == 'N') {
            cout << "Exiting game loop." << endl;
            break;
        } else if (proceed == 'y' || proceed == 'Y') {
            currentRobot->actions(&battlefield);  // Call the robot's action sequence
        } else {
            cout << "Wrong input, skipping your turn." << endl;
        }

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