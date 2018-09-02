#include <thread>
#include <chrono>
#include <boost/program_options.hpp>
#include <string>
#include <iostream>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

struct config {
    bool help_wanted = false;
    std::string path;
    std::chrono::duration<double> period;
};

config parse_config(int ac, char** av) {
    namespace bpo = boost::program_options;

    config cfg;
    double period = 0.1;
    bpo::options_description desc("trickle-fsync options");
    desc.add_options()
	("path", bpo::value<std::string>(&cfg.path),
	 "path to mountpoint to periodically fsync")
	("period", bpo::value<double>(&period)->default_value(0.1f),
	 "fsync period (in seconds)")
	("help", "produce this help message")
	;
    bpo::positional_options_description pod;
    pod.add("path", 1);
    pod.add("period", 1);
    bpo::variables_map vm;
    bpo::store(bpo::command_line_parser(ac, av).options(desc).positional(pod).run(),
	       vm);
    bpo::notify(vm);
    cfg.period = std::chrono::duration<double>(period);
    if (vm.count("help") || !vm.count("path")) {
	std::cout << "Usage: " << av[0] << " MOUNTPOINT [PERIOD]\n";
	std::cout << desc << "\n";
	cfg.help_wanted = true;
    }
    return cfg;
}

int main(int ac, char** av) {
    auto cfg = parse_config(ac, av);
    if (cfg.help_wanted) {
	return 1;
    }
    auto templat = cfg.path + "/trickle-fsync.XXXXXX";
    std::vector<char> vt(templat.data(), templat.data() + templat.size() + 1);
    int fd = mkstemp(vt.data());
    if (fd == -1) {
	perror("mkstemp");
	return 2;
    }
    unlink(vt.data());
    char buf[1] = {};
    while (true) {
	auto now = std::chrono::steady_clock::now();
	auto err1 = pwrite(fd, buf, 1, 0);
	if (err1 != 1) {
	    perror("pwrite");
	    exit(3);
	}
	++buf[0];
	auto err2 = fdatasync(fd);
	if (err2 != 0) {
	    perror("fdatasync");
	    exit(4);
	}
	std::this_thread::sleep_until(now + cfg.period);
    }
}
