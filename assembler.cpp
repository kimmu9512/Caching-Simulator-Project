// assembler.cpp

#include <iostream>
#include <fstream>
#include <vector>
#include <memory>
#include <unordered_map>
#include <sstream>
#include <algorithm>
#include <stdexcept>

// Constants
constexpr int WORD_SIZE = 2;
constexpr int CODE_SIZE = 1024 * WORD_SIZE;
constexpr int LABEL_SIZE = 28;

// Enums
enum class Opcode {
    ADD, SUB, AND, OR, XOR, MOVE, SHIFT, BRANCH
};

// Interfaces
class IInstruction {
public:
    virtual ~IInstruction() = default;
    virtual std::vector<unsigned char> encode() const = 0;
};

class IAssembler {
public:
    virtual ~IAssembler() = default;
    virtual std::vector<unsigned char> assemble(const std::vector<std::string>& sourceCode) = 0;
};

// Implementations
class Instruction : public IInstruction {
protected:
    Opcode opcode;
    unsigned char type;
    unsigned char reg1;
    unsigned char reg2;
    short immediate;

public:
    Instruction(Opcode op, unsigned char t, unsigned char r1, unsigned char r2, short imm)
        : opcode(op), type(t), reg1(r1), reg2(r2), immediate(imm) {}

    std::vector<unsigned char> encode() const override {
        std::vector<unsigned char> encoded(2);
        encoded[0] = (static_cast<unsigned char>(opcode) << 5) | (type << 2) | (reg1 >> 2);
        encoded[1] = ((reg1 & 0x03) << 6) | (reg2 << 2) | (immediate & 0x03);
        return encoded;
    }
};

class Assembler : public IAssembler {
private:
    std::unordered_map<std::string, unsigned short> labelAddresses;
    std::vector<std::unique_ptr<IInstruction>> instructions;

    Opcode getOpcode(const std::string& op) {
        static const std::unordered_map<std::string, Opcode> opcodeMap = {
            {"ADD", Opcode::ADD}, {"SUB", Opcode::SUB}, {"AND", Opcode::AND},
            {"OR", Opcode::OR}, {"XOR", Opcode::XOR}, {"MOVE", Opcode::MOVE},
            {"SHIFT", Opcode::SHIFT}, {"BRANCH", Opcode::BRANCH}
        };
        auto it = opcodeMap.find(op);
        if (it == opcodeMap.end()) {
            throw std::runtime_error("Invalid opcode: " + op);
        }
        return it->second;
    }

    unsigned char getRegister(const std::string& reg) {
        if (reg[0] != 'R' || reg.length() < 2) {
            throw std::runtime_error("Invalid register: " + reg);
        }
        return static_cast<unsigned char>(std::stoi(reg.substr(1)));
    }

    void parseInstruction(const std::string& line, unsigned short address) {
        std::istringstream iss(line);
        std::string token;
        iss >> token;

        if (token.back() == ':') {
            // This is a label
            token.pop_back();
            labelAddresses[token] = address;
            iss >> token; // Get the next token (should be an opcode)
        }

        Opcode opcode = getOpcode(token);
        unsigned char type = 0;
        unsigned char reg1 = 0;
        unsigned char reg2 = 0;
        short immediate = 0;

        // Parse operands based on opcode
        switch (opcode) {
            case Opcode::ADD:
            case Opcode::SUB:
            case Opcode::AND:
            case Opcode::OR:
            case Opcode::XOR:
                iss >> token;
                reg1 = getRegister(token);
                iss >> token;
                if (token[0] == 'R') {
                    reg2 = getRegister(token);
                    type = 1;
                } else {
                    immediate = std::stoi(token);
                }
                break;
            case Opcode::MOVE:
                // Implement MOVE parsing
                break;
            case Opcode::SHIFT:
                // Implement SHIFT parsing
                break;
            case Opcode::BRANCH:
                // Implement BRANCH parsing
                break;
        }

        instructions.push_back(std::make_unique<Instruction>(opcode, type, reg1, reg2, immediate));
    }

public:
    std::vector<unsigned char> assemble(const std::vector<std::string>& sourceCode) override {
        instructions.clear();
        labelAddresses.clear();

        // First pass: collect labels and parse instructions
        unsigned short address = 0;
        for (const auto& line : sourceCode) {
            if (!line.empty() && line[0] != ';') {
                parseInstruction(line, address);
                address += 2; // Each instruction is 2 bytes
            }
        }

        // Second pass: resolve labels and encode instructions
        std::vector<unsigned char> machineCode;
        for (const auto& instr : instructions) {
            auto encoded = instr->encode();
            machineCode.insert(machineCode.end(), encoded.begin(), encoded.end());
        }

        return machineCode;
    }
};

class FileHandler {
public:
    static std::vector<std::string> readFile(const std::string& filename) {
        std::ifstream file(filename);
        if (!file) {
            throw std::runtime_error("Unable to open file: " + filename);
        }
        std::vector<std::string> lines;
        std::string line;
        while (std::getline(file, line)) {
            lines.push_back(line);
        }
        return lines;
    }

    static void writeFile(const std::string& filename, const std::vector<unsigned char>& data) {
        std::ofstream file(filename, std::ios::binary);
        if (!file) {
            throw std::runtime_error("Unable to create file: " + filename);
        }
        file.write(reinterpret_cast<const char*>(data.data()), data.size());
    }
};

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file.asm>" << std::endl;
        return 1;
    }

    try {
        std::vector<std::string> sourceCode = FileHandler::readFile(argv[1]);
        
        auto assembler = std::make_unique<Assembler>();
        std::vector<unsigned char> machineCode = assembler->assemble(sourceCode);

        std::string outputFilename = argv[1];
        outputFilename = outputFilename.substr(0, outputFilename.find_last_of('.')) + ".o";
        FileHandler::writeFile(outputFilename, machineCode);

        std::cout << "Assembly successful. Output written to " << outputFilename << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}