// $Id$

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "CliExtension.hh"
#include "MSXConfig.hh"
#include "FileOperations.hh"
#include "FileContext.hh"


CliExtension::CliExtension()
{
	CommandLineParser::instance()->
		registerOption(std::string("-ext"), this);

	SystemFileContext context;
	const std::list<std::string> &paths = context.getPaths();
	std::list<std::string>::const_iterator it;
	for (it = paths.begin(); it != paths.end(); it++) {
		std::string path = FileOperations::expandTilde(*it);
		createExtensions(path + "share/extensions/");
	}
}

CliExtension::~CliExtension()
{
}

void CliExtension::parseOption(const std::string &option,
                               std::list<std::string> &cmdLine)
{
	const std::string &extension = cmdLine.front();
	std::map<std::string, std::string>::const_iterator it =
		extensions.find(extension);
	if (it != extensions.end()) {
		MSXConfig *config = MSXConfig::instance();
		config->loadFile(new SystemFileContext(), it->second);
	} else {
		PRT_ERROR("Extension \"" << extension << "\" not found!");
	}
		
	cmdLine.pop_front();
}

const std::string& CliExtension::optionHelp() const
{
	static std::string help("Insert the extension specified in argument");
	return help;
}

static int select(const struct dirent* d)
{
	struct stat s;
	// entry must be a directory
	if (stat(d->d_name, &s)) {
		return 0;
	}
	if (!S_ISDIR(s.st_mode)) {
		return 0;
	}

	// directory must contain the file "hardwareconfig.xml"
	std::string file(std::string(d->d_name) + "/hardwareconfig.xml");
	if (stat(file.c_str(), &s)) {
		return 0;
	}
	if (!S_ISREG(s.st_mode)) {
		return 0;
	}
	return 1;
}

void CliExtension::createExtensions(const std::string &path)
{
	char buf[PATH_MAX];
	if (!getcwd(buf, PATH_MAX)) {
		return;
	}
	if (chdir(path.c_str())) {
		return;
	}
	struct dirent **namelist;
	int n = scandir(".", &namelist, select, 0);
	while ((n--) > 0) {
		std::string optionName(namelist[n]->d_name);
		std::string optionPath(path + optionName + "/hardwareconfig.xml");
		PRT_DEBUG("Extension: " << optionName << " " << optionPath);
		extensions[optionName] = optionPath;
		free(namelist[n]);
	}
	free (namelist);
	chdir(buf);
}
