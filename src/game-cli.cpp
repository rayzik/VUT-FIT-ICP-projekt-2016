/**
 * @file   console.cpp
 * @Author Jan Silhan (xsilha10@stud.fit.vutbr.cz), Pavel Pospisil (xpospi88@stud.fit.vutbr.cz)
 * @date   may 2016
 * @brief  work with console in both systems
 *
 * sometimes i believe compiler ignores all my comments, but doxygen don't
 */

#include "game-cli.h"

typedef struct fileLoad{
    std::time_t time;
    std::string basename;


} fileInfo;

bool fileSort (fileInfo i,fileInfo j) { return std::greater<std::time_t>()(i.time, j.time); }

gameCli::gameCli()
:game(){
    consol.clear();
    printHelp();
    getGameInfo();
    message = "";
}

gameCli::~gameCli(){

}

void gameCli::getGameInfo(){

    int single = -1;
    std::string a;
    while(true){
        consol.clear();
        if(single != -1)
            std::cout<< "Please enter valid game type 1 or 2"<<std::endl;

        std::cout<<"1) Single Player"<<std::endl
        <<"2) Multiplayer"<<std::endl
        <<"Please enter game type: ";


        std::getline(std::cin, a);
        single = std::stoi("0"+a);

        if(single == 1 || single == 2)
            break;
    
    }
    initPlayers("Player 1", "Player 2",single == 1);
    
    if(player2->computer){
        while(true){
            consol.clear();
            std::cout<< "select difficulty"<<std::endl;
            std::cout<<"1) Easy"<<std::endl
            <<"2) Super Uber Hard"<<std::endl
            <<"Please enter game type: ";


            std::getline(std::cin, a);
            single = std::stoi("0"+a);

            if(single == 1 || single == 2)
                break;
        }
        easyComputer = single == COMPUTEREASY;
    
    }
    


    int tempSize = -1;
    while(true){
        consol.clear();
        std::cout<< "Please enter valid size 6, 8, 10, 12. Or press enter or type 0 to set default size"<<std::endl;
        std::cout<<"Enter game field size: ";
        std::getline(std::cin, a);
        tempSize = std::stoi("0"+a);

        if((tempSize % 2) == 0 && tempSize > 5 && tempSize < 13 )
            break;
        if(tempSize == 0)
        {
            tempSize = 8;
            break;
        }

    }

    size = tempSize;
    initGameField();

}

void gameCli::drawScore(){

    std::cout << player1->getName() << ": " << player1->getScore() << " ";
    consol.setFgColor(CSYELLOW);
    consol.setBgColor(CSBLACK);
    std::cout<<"O";

    consol.resetToDefault();
    std::cout << "\t" << player2->getName() << ": " << player2->getScore() << " ";
    consol.setFgColor(CSBLACK);
    consol.setBgColor(CSWHITE);
    std::cout<<"O";

    consol.resetToDefault();
    std::cout<<std::endl;
}

void gameCli::draw(){

    consol.clear();
    if(!message.empty())
        std::cout<<message<<std::endl;
    consol.setFgColor(CSBLACK);
    consol.setBgColor(CSWHITE);


    std::cout << "  ";
    if(size>9)
        std::cout<<" ";
    for(int firstLine = 0; firstLine < size; firstLine++)
        std::cout << (char)(97 + firstLine) << " ";
    std::cout << std::endl;


    for (int x = 0; x < size; x++)
    {
        std::cout << (x+1) << " " << ((size>9 && x < 9)?" ":"");
        for (int y = 0; y < size; y++)
        {
            if(gameField[x][y] == EMPTY)
                std::cout << "_";
            else{
                consol.setFgColor(gameField[x][y] == BLACK? CSYELLOW: CSBLACK);
                consol.setBgColor(gameField[x][y] == BLACK? CSBLACK : CSWHITE);
                std::cout << ("O");
                consol.setFgColor(CSBLACK);
                consol.setBgColor(CSWHITE);
            }
            std::cout << "|";
        }
        std::cout << std::endl;
    }

    consol.resetToDefault();

    std::cout << std::endl;
    consol.setCursor(size*2 + 5, 5);

    std::cout << "Now Playing: ";
    consol.setFgColor(actualPlayer1? CSYELLOW : CSBLACK );
    consol.setBgColor(actualPlayer1? CSBLACK: CSWHITE );
    std::cout<< "O";
    consol.resetToDefault();
    consol.setCursor(0, size*2+6);
    drawScore();
}

void gameCli::printGameOver(){
    //Todo print game over
}

void gameCli::printSavedGame(){
    consol.clear();
    boost::filesystem::path dir("saves");
    boost::filesystem::create_directory(dir);

    std::stringstream filename;

    std::vector<fileInfo> v;

    for(auto& entry : boost::make_iterator_range(boost::filesystem::directory_iterator(dir), {}))
    {
        std::time_t t = boost::filesystem::last_write_time(entry);
        std::stringstream ss;
        ss << boost::filesystem::basename(entry);
        fileInfo fi;
        fi.time = t;
        fi.basename = ss.str();
        v.insert(v.begin(),fi);
    }

    std::sort(v.begin(), v.end(), fileSort);

    int i=1;
    for (std::vector<fileInfo>::iterator it=v.begin(); it!=v.end(); ++it, i++){
        char buff[20];
        strftime(buff, 20, "%Y-%m-%d %H:%M", localtime(&(it->time)));
        std::cout<< i <<") "<< it->basename << " " << buff <<std::endl;
    }
    int selected = 0;
    std::string a;
    std::getline(std::cin, a);

    while(true){


        std::cout<<"Please enter number of file "<<std::endl<<"or press enter to load last game: ";
        std::getline(std::cin, a);


        std::size_t found;
        if((found = a.find("return"))!= std::string::npos){
            return;
        }
        else{
            selected = std::stoi("0" + a);
            if(selected < 0 || selected > i)
                continue;
            if(selected != 0)
                selected--;

            if(loadGame(v[selected].basename + ".sav")){
                message = "Load was successful";
                std::cin.get();
            }
            else
                message = "Load was unsuccessful";
            return;
        }

    }

}


void gameCli::run(){

    while (true)
    {
        if(!actualPlayer1 && player2->computer)
        {
            computerMove();
            continue;
        }
        int possible;
        switch(possible = impossibleMove()){
            case 1:
                 actualPlayer1 = !actualPlayer1;
                 message = "You were skipped";
            break;
            case 2: 
                printGameOver();
                getGameInfo();
                
            break;
        }

        if(possible > 0)
            continue;

        std::string command;

        draw();
        message = "";



        std::cin >> command;


        std::size_t found;

        if((found = command.find("quit"))!= std::string::npos){
            return;
        }
        else if((found = command.find("back"))!= std::string::npos){
            prevStep();
            // draw();
            continue;
        }
        else if((found = command.find("forward"))!= std::string::npos){
            nextStep();            
            // draw();
            continue;
        }
        else if((found = command.find("help"))!= std::string::npos){
            consol.clear();
            printHelp();
            std::cin.get();
        }
        else if((found = command.find("load")) != std::string::npos){
            consol.clear();
            printSavedGame();
        }
        else if((found = command.find("save")) != std::string::npos){
            if(!saveGame())
                message = "Save Unsuccessful!!";
            else
                message = "Save successful!";
        }
        else{

            
            std::size_t i = -1;
            std::string rowNum = "";

            while (isdigit(command[++i]))
            {
                rowNum += command[i];
            }
            int row = std::stoi("0" + rowNum) -1;

            command.erase(0, i);
            int col = (int)(command[0] - 97);

            if(!makeMove(WRITE,row,col))
                message = "Invalid move!";

        }

    }
}

void gameCli::printHelp(){
    std::cout << "HRA2016 Help" <<std::endl
    << "To help write help" << std::endl
    << "To quit write quit" << std::endl
    << "to save game type save" << std::endl
    << "to load game type load" << std::endl
    << "To make move just write <row><column>" <<std::endl
    << "to continue to game press enter..." << std::endl;

    std::cin.get();
}


