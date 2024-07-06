#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stack>
#include <string>
#include <unordered_map>


#ifdef _WIN32
#define OS_WINDOWS
#elif __APPLE__
#define OS_MAC
#elif __linux__
#define OS_LINUX
#endif

void compileSource(const std::string &inputFile,
                   const std::string &outputFile) {
  std::ifstream inFile(inputFile);
  std::ofstream outFile(outputFile);

  if (!inFile || !outFile) {
    std::cerr << "Error opening file." << std::endl;
    return;
  }

  std::string line;
  int labelCounter = 0;
  std::stack<int> loopLabels;
  std::stack<int> ifLabels;

  // Ecriture des sections .data et .text
  outFile << "section .data align=8\n";
  std::unordered_map<std::string, bool> globalVars;
  std::unordered_map<std::string, bool> localVars;
  bool inFunction = false;

  outFile << "section .text align=16\n";
  outFile << "global main\n";

  while (std::getline(inFile, line)) {
    // Trim whitespace from both ends of the line
    line.erase(0, line.find_first_not_of(" \t\n\r\f\v"));
    line.erase(line.find_last_not_of(" \t\n\r\f\v") + 1);

    if (line.substr(0, 5) == "func ") {
      std::string funcName =
          line.substr(5, line.size() - 6); // Remove "func " and trailing {
      outFile << funcName << ":\n";
      inFunction = true;
    } else if (line == "}") {
      inFunction = false;
      localVars.clear(); // Clear local variables after leaving function
    } else if (line.substr(0, 4) == "var ") {
      std::string varName = line.substr(4);
      if (inFunction) {
        if (localVars.find(varName) == localVars.end()) {
          localVars[varName] = true;
          outFile << varName
                  << " resb 8\n"; // Reserve 8 bytes for local variable
        }
      } else {
        if (globalVars.find(varName) == globalVars.end()) {
          globalVars[varName] = true;
          outFile << varName
                  << " resb 8\n"; // Reserve 8 bytes for global variable
        }
      }
    } else if (line.substr(0, 7) == ".while ") {
      std::string condition = line.substr(
          7, line.size() - 8); // Remove ".while " and closing parenthesis
      outFile << "L" << labelCounter << "_start:\n";
      outFile << "    cmp " << condition << "\n";
      outFile << "    je L" << labelCounter << "_end\n";
      loopLabels.push(labelCounter);
      labelCounter++;
    } else if (line == ".endwhile") {
      int startLabel = loopLabels.top();
      loopLabels.pop();
      outFile << "    jmp L" << startLabel << "_start\n";
      outFile << "L" << startLabel << "_end:\n";
    } else if (line.substr(0, 4) == ".if ") {
      std::string condition = line.substr(
          4, line.size() - 5); // Remove ".if " and closing parenthesis
      outFile << "    cmp " << condition << "\n";
      outFile << "    jne L" << labelCounter << "_else\n";
      ifLabels.push(labelCounter);
      labelCounter++;
    } else if (line.substr(0, 6) == ".elif ") {
      int elseLabel = ifLabels.top();
      outFile << "    jmp L" << elseLabel << "_end\n";
      outFile << "L" << elseLabel << "_else:\n";
      std::string condition = line.substr(
          6, line.size() - 7); // Remove ".elif " and closing parenthesis
      outFile << "    cmp " << condition << "\n";
      outFile << "    jne L" << elseLabel << "_end\n";
    } else if (line == ".endif") {
      int ifLabel = ifLabels.top();
      ifLabels.pop();
      outFile << "L" << ifLabel << "_end:\n";
    } else if (line.substr(0, 5) == "call ") {
      std::string funcName = line.substr(5);
      outFile << "    call " << funcName << "\n";
    } else {
      outFile << "    " << line
              << "\n"; // Directly copy the line for other instructions
    }
  }

  inFile.close();
  outFile.close();
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    std::cerr << "[wys.compiler]: Usage: " << argv[0]
              << " <input file> <output file>" << std::endl;
    return 1;
  }

  compileSource(argv[1], argv[2]);

  std::cout << "[wys.compiler]: compiled\n";
  std::cout << "[wys.compiler]: <out_of_control>: compiling & linking with "
               "gcc, nasm\n";

  std::string command;
  std::string outputFile = argv[2];
  std::string executableFile =
      outputFile.substr(0, outputFile.find_last_of('.'));

#ifdef OS_WINDOWS
  command = "nasm -f win64 " + outputFile + " -o " + executableFile +
            ".obj && gcc " + executableFile + ".obj -o " + executableFile +
            ".exe";
#elif defined(OS_MAC)
  command = "nasm -f macho64 " + outputFile + " -o " + executableFile +
            ".o && gcc " + executableFile + ".o -o " + executableFile;
#elif defined(OS_LINUX)
  command = "nasm -f elf64 " + outputFile + " -o " + executableFile +
            ".o && gcc " + executableFile + ".o -o " + executableFile;
#else
  std::cerr << "Unsupported operating system." << std::endl;
  return 1;
#endif

  int result = system(command.c_str());
  if (result != 0) {
    std::cerr << "Error during assembly and linking." << std::endl;
    return 1;
  }

  std::cout << "[wys.compiler]: successfully compiled and linked to "
            << executableFile << "\n";

  return 0;
}
