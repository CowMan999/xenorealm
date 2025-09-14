#include <filesystem>
#include <fstream>
#include <csignal>

#include "game.hpp"

void sig(int code) {

	// log
	std::ofstream file("./data/log.dat", std::ios_base::app);
	if(code == SIGSEGV)
		file << time(nullptr) << " Segmentation fault"  << '\n';
	else if(code == SIGTERM || code == SIGINT || code == SIGILL)
		file << time(nullptr) << " Unexpected external termination"  << '\n';
	else
		file << time(nullptr) << " Unexpected error " << code << '\n';

	file.close();
	std::terminate();
}


int main(int argc, char* argv[]) 
{
	(void)argc;

	// setup signal handlers
	signal(SIGSEGV, sig); 
	signal(SIGTERM, sig); 
	signal(SIGINT, sig); 
	signal(SIGILL, sig); 
	signal(SIGFPE, sig); 

    // reset directory
    std::filesystem::current_path(std:: filesystem::path(argv[0]).parent_path());
    
    // create game data folder if it doesn't exist 
    if(!std::filesystem::exists("./data"))
    {
        std:: filesystem::create_directory("./data");
    }
    
    // create game log file
    if(!std::filesystem::exists("./data/log.dat"))
    {
        std::ofstream log("./data/log.dat");
        log << GAME_NAME << " Log File\n";
        log.close();
    }
    
    try {
        Game game;
        game.mainloop();
    }
    catch(const std::exception& e) {
        
        // shove error in log file and terminate
        std::ofstream log("./data/log.dat", std::ios::app);
        log << time(nullptr) << ' ' << e.what() << '\n';
        log.close();
        
        std::terminate();
    }
}