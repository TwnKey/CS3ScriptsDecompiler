#ifndef CS3INSTRUCTIONSSET_H
#define CS3INSTRUCTIONSSET_H
#include "utilities.h"
#include "functions.h"
#include "translationfile.h"

#include <QString>
int add_operandes(int &addr, QByteArray &content, Instruction * instr, int size){
    int plVar15 = addr;
    int lVar5, lVar7;
    lVar7 = -1;
    do {
      instr->AddOperande(operande(addr,"byte", ReadSubByteArray(content, addr,1)));
      lVar5 = lVar7 + size;
      lVar7 = lVar7 + 1;
    } while (content[plVar15+lVar5] != '\0');
    return lVar7;


}
void fun_140498b70(int &addr, QByteArray &content, Instruction * instr){
    //related to text/text formating
    QByteArray current_op_value;
    int addr_ = addr;
    bool start = false;
    bool start_text = false;
    int cnt = 0;
    do{
        unsigned char current_byte = content[addr];
        qDebug() << "current byte: " << hex << current_byte << " at addr: " << addr;
        if (current_byte<0x20){
            if (current_byte == 0){
                current_op_value.clear();
                current_op_value[0] = 0;
                instr->AddOperande(operande(addr,"byte", current_op_value));
                addr++;
                return;
            }
            else if (current_byte == 0x10){
                addr++;
                instr->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));
            }
            else if ((current_byte > 0x10)&&(current_byte < 0x13)){
                addr++;
                instr->AddOperande(operande(addr,"int", ReadSubByteArray(content, addr,4)));
            }
            else if (current_byte > 0x2){
                start_text = false;
                instr->AddOperande(operande(addr_,"string", current_op_value));
                current_op_value.clear();
                current_op_value.push_back(current_byte);
                instr->AddOperande(operande(addr,"byte", current_op_value));
                addr++;


            }
            else{
                current_op_value.clear();
                current_op_value.push_back(current_byte);
                instr->AddOperande(operande(addr,"byte", current_op_value));
                addr++;
            }

        }
        else{
            if (current_byte == 0x23){
                addr_ = addr;//beginning of some format stuff
                current_op_value.push_back(0x23);
                addr++;
                current_byte = content[addr];
                if (start) start = false;
                else {
                    int scnd_cnt = 0;
                    do{
                        if ((current_byte == 0x00)||(current_byte == 0x46)||(current_byte == 0x6b)||(current_byte == 0x4b)) {
                            addr++;
                            current_op_value.push_back(current_byte);
                            instr->AddOperande(operande(addr_,"bytearray", current_op_value));
                            current_op_value.clear();
                            break;
                        }
                        if (current_byte == 0x52) start = true;


                        current_op_value.push_back(current_byte);
                        addr++;
                        current_byte = content[addr];
                        scnd_cnt++;


                    }
                    while (scnd_cnt <2);

                }
                current_op_value.push_back(current_byte);
                instr->AddOperande(operande(addr_,"bytearray", current_op_value));
                current_op_value.clear();
            }
            else{
                //here, should be actual text (I think)
                if (!start_text) {
                    start_text = true;
                    addr_ = addr;
                }
                current_op_value.push_back(current_byte);
                addr++;

            }

        }
        cnt++;
    }
    while(cnt<9999);

}
void fun_1403c90e0(int &addr, QByteArray &content, Instruction * instr, int param){
    QByteArray control_byte3_arr = ReadSubByteArray(content, addr, 1);
    instr->AddOperande(operande(addr,"byte", control_byte3_arr));
    unsigned char control_byte3 = (unsigned char)control_byte3_arr[0];

    if (control_byte3 == '\x11'){
        instr->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));
        instr->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));

    }
    else if (control_byte3 != '3'){

        if (control_byte3 == '\"'){
            instr->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));
            instr->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));

        }
        else if (control_byte3 != 0x44){

            if (control_byte3 == 0xDD){
                qDebug() << "reading string starting at " << hex << addr;
                //there is a XOR,XOR EDI which causes a jump that ignores the strcpy
                if (param!=0)instr->AddOperande(operande(addr,"string", ReadStringSubByteArray(content, addr)));
                instr->AddOperande(operande(addr,"string", ReadStringSubByteArray(content, addr)));
            }
            else if (control_byte3 == 0xFF){
                instr->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));
                instr->AddOperande(operande(addr,"byte", ReadSubByteArray(content, addr,1)));
            }
            else{
                if (control_byte3 != 0xEE){

                }
                else{
                    instr->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));
                    instr->AddOperande(operande(addr,"byte", ReadSubByteArray(content, addr,1)));
                }
            }


        }

    }
}
void sub05(int &addr, QByteArray &content, Instruction * instr){
    QByteArray control_byte = ReadSubByteArray(content, addr, 1);
    instr->AddOperande(operande(addr,"byte", control_byte));

    while ((int)control_byte[0]!=1){

            switch ((unsigned char)control_byte[0])  {
                case 0x0:
                    instr->AddOperande(operande(addr,"int", ReadSubByteArray(content, addr, 4)));
                    break;

                case 0x1c:{
                    //the next byte is the OP code for a new instruction
                    QByteArray OPCode_byte = ReadSubByteArray(content, addr, 1);
                    instr->AddOperande(operande(addr,"byte", OPCode_byte));
                    std::shared_ptr<Instruction> instr2 = instr->Maker->CreateInstructionFromDAT(addr, content);
                    instr->AddOperande(operande(addr,"bytearray", instr2->getBytes()));
                    break;
                }


                case 0x1e:
                    instr->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));
                    break;
                case 0x1f:
                case 0x20:
                case 0x23:

                    instr->AddOperande(operande(addr,"byte", ReadSubByteArray(content, addr, 1)));
                    break;
                case 0x21:
                    instr->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));
                    break;
                default:
                    ;
            }

            control_byte = ReadSubByteArray(content, addr, 1);
            instr->AddOperande(operande(addr,"byte", control_byte));

        }
}
class CreateMonsters : public Instruction
{
    public:
    CreateMonsters():Instruction(256,nullptr){}
    CreateMonsters(Builder *Maker):Instruction("CreateMonsters", 256, Maker){}
    CreateMonsters(int &addr, QByteArray &content,Builder *Maker):Instruction("CreateMonsters", 256,Maker){
        int initial_addr = addr;
        this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr, 4)));
        this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr, 4)));
        this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr, 4)));
        this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr, 4)));

        this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr, 4)));
        this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr, 4)));
        this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr, 4)));
        this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr, 4)));
        this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr, 4)));
        this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr, 4)));

        this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr, 2))); //RSI + 0x28
        this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr, 2)));

        this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr, 4))); //0x2C

        this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr, 2)));//0x30
        this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr, 2)));//0x32
        this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr, 2)));//0x34
        this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr, 2)));//0x36

        this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr, 4)));//0x38

        this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr, 4)));//0x3C
        this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr, 4)));//0x40
        this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr, 4)));//0x44
        this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr, 4)));//0x48
        //0x4C The first part is done
        //from here, it's just a guess. There is a maximum of 8 enemies hardcoded in the function.
        QByteArray array = ReadSubByteArray(content, addr, 4);
        this->AddOperande(operande(addr,"int", array));
        int first = ReadIntegerFromByteArray(0,array);
        do{
            int counter = 0;

            do{
                counter++;

                QByteArray monsters_name = ReadSubByteArray(content, addr, 16); //I believe the name of the monsters has to be 8 bytes long,
                //but I saw a check for the \x0 character so I include the additional 0 and will deal with them later
                this->AddOperande(operande(addr,"string", monsters_name));
            }
            while(counter < 0x7);
            for (int idx_byte = 0; idx_byte < 8; idx_byte++) this->AddOperande(operande(addr,"byte", ReadSubByteArray(content, addr, 1)));
            this->AddOperande(operande(addr,"bytearray", ReadSubByteArray(content, addr, 8))); //??

            QByteArray array = ReadSubByteArray(content, addr, 4);
            this->AddOperande(operande(addr,"int", array));
            first = ReadIntegerFromByteArray(0,array);

        }
        while(first != -1);
        this->AddOperande(operande(addr,"bytearray", ReadSubByteArray(content, addr, 0x18))); //??
        this->AddOperande(operande(addr,"int", ReadSubByteArray(content, addr, 4)));
    }


};
class OPCode0 : public Instruction
{
    public:
    OPCode0():Instruction(0,nullptr){}
    OPCode0(Builder *Maker):Instruction("Instruction 0", 0, Maker){}
    OPCode0(int &addr, QByteArray &content,Builder *Maker):Instruction("Instruction 0", 0,Maker){
        addr++;

        QByteArray control_byte = ReadSubByteArray(content, addr, 1);
        this->AddOperande(operande(addr,"byte", control_byte));
    }


};
class OPCode1 : public Instruction
{
    public:
    OPCode1():Instruction(1,nullptr){}
    OPCode1(Builder *Maker):Instruction("Return",1,Maker){}
    OPCode1(int &addr, QByteArray &content, Builder *Maker):Instruction("Return", 1,Maker){

    }


};
class OPCode2 : public Instruction
{
    public:
    OPCode2():Instruction(2,nullptr){}
    OPCode2(Builder *Maker):Instruction("Call",2,Maker){}
    OPCode2(int &addr, QByteArray &content, Builder *Maker):Instruction("Call", 2,Maker){
            addr++;

            QByteArray control_byte = ReadSubByteArray(content, addr, 1);
            this->AddOperande(operande(addr,"byte", control_byte));
            QByteArray function_name = ReadStringSubByteArray(content, addr);
            this->AddOperande(operande(addr,"string", function_name));
            QByteArray control_byte2_arr = ReadSubByteArray(content, addr, 1);
            this->AddOperande(operande(addr,"byte", control_byte2_arr));

            int control_byte2 = (int)control_byte2_arr[0];

            if (control_byte2!=0){
                for (int i = 0; i < control_byte2; i++){
                    fun_1403c90e0(addr, content, this, 0);
                }

            }
            switch((unsigned char)control_byte[0]){
                case 0xB:

                    qDebug() << "Calling function " << ConvertBytesToString(function_name);
                    //here we're calling a function that was defined at the beginning
                    std::vector<function>::iterator it = find_function_by_name(Maker->FunctionsToParse,ConvertBytesToString(function_name));
                    qDebug() << "found " << it->name;
                    if (!std::count(Maker->FunctionsParsed.begin(), Maker->FunctionsParsed.end(), *it)){
                        Maker->ReadIndividualFunction(*it,content);
                        Maker->FunctionsParsed.push_back(*it);
                    }
                    //Maker->FunctionsToParse.erase(it);
                    break;
            }



    }


};
class OPCode3 : public Instruction
{
    public:
    OPCode3():Instruction(3,nullptr){}
    OPCode3(Builder *Maker):Instruction("???",3,Maker){}
    OPCode3(int &addr, QByteArray &content, Builder *Maker):Instruction("???", 3,Maker){
        addr++;
        this->AddOperande(operande(addr,"pointer", ReadSubByteArray(content, addr, 4)));
    }


};
class OPCode5 : public Instruction
{
    public:
    OPCode5():Instruction(5,nullptr){}
    OPCode5(Builder *Maker):Instruction("???",5,Maker){}
    OPCode5(int &addr, QByteArray &content, Builder *Maker):Instruction("???", 5,Maker){
            addr++;
            sub05(addr, content, this);
            this->AddOperande(operande(addr,"pointer", ReadSubByteArray(content, addr, 4))); // pointer
    }
};
class OPCodeC : public Instruction
{
    public:
    OPCodeC():Instruction(0xC,nullptr){}
    OPCodeC(Builder *Maker):Instruction("???",0xC,Maker){}
    OPCodeC(int &addr, QByteArray &content, Builder *Maker):Instruction("???", 0xC,Maker){
            addr++;
            this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));

    }
};
class OPCode10 : public Instruction
{
    public:
    OPCode10():Instruction(0x10,nullptr){}
    OPCode10(Builder *Maker):Instruction("???",0x10,Maker){}
    OPCode10(int &addr, QByteArray &content, Builder *Maker):Instruction("???", 0x10,Maker){
            addr++;
            this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));

    }
};
class OPCode11 : public Instruction
{
    public:
    OPCode11():Instruction(0x11,nullptr){}
    OPCode11(Builder *Maker):Instruction("???",0x11,Maker){}
    OPCode11(int &addr, QByteArray &content, Builder *Maker):Instruction("???", 0x11,Maker){
            addr++;
            this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));

    }
};
class OPCode13 : public Instruction
{
    public:
    OPCode13():Instruction(0x13,nullptr){}
    OPCode13(Builder *Maker):Instruction("???",0x13,Maker){}
    OPCode13(int &addr, QByteArray &content, Builder *Maker):Instruction("???", 0x13,Maker){
            addr++;
            this->AddOperande(operande(addr,"int", ReadSubByteArray(content, addr,4)));

    }
};
class OPCode14 : public Instruction
{
    public:
    OPCode14():Instruction(0x14,nullptr){}
    OPCode14(Builder *Maker):Instruction("???",0x14,Maker){}
    OPCode14(int &addr, QByteArray &content, Builder *Maker):Instruction("???", 0x14,Maker){
            addr++;
            this->AddOperande(operande(addr,"int", ReadSubByteArray(content, addr,4)));

    }
};
class OPCode16 : public Instruction
{
    public:
    OPCode16():Instruction(0x16,nullptr){}
    OPCode16(Builder *Maker):Instruction("???",0x16,Maker){}
    OPCode16(int &addr, QByteArray &content, Builder *Maker):Instruction("???", 0x16,Maker){
            addr++;
            this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));

    }
};
class OPCode18 : public Instruction
{
    public:
    OPCode18():Instruction(0x18,nullptr){}
    OPCode18(Builder *Maker):Instruction("???",0x18,Maker){}
    OPCode18(int &addr, QByteArray &content, Builder *Maker):Instruction("???", 0x18,Maker){
            addr++;
            sub05(addr, content, this);

    }
};
class OPCode1D : public Instruction
{
    public:
    OPCode1D():Instruction(0x1D,nullptr){}
    OPCode1D(Builder *Maker):Instruction("???",0x1D,Maker){}
    OPCode1D(int &addr, QByteArray &content, Builder *Maker):Instruction("???", 0x1D,Maker){
            addr++;
            this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));
            this->AddOperande(operande(addr,"string", ReadStringSubByteArray(content, addr)));
            this->AddOperande(operande(addr,"string", ReadStringSubByteArray(content, addr)));
            this->AddOperande(operande(addr,"string", ReadStringSubByteArray(content, addr)));
            this->AddOperande(operande(addr,"byte", ReadSubByteArray(content, addr,1)));
            this->AddOperande(operande(addr,"int", ReadSubByteArray(content, addr,4)));
            this->AddOperande(operande(addr,"int", ReadSubByteArray(content, addr,4)));//6
            this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));//10
            this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));//E
            this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));//0x12
            this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));//0x16
            this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));//0x1A
            this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));//0x1E
            this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));//0x22
            this->AddOperande(operande(addr,"string", ReadStringSubByteArray(content, addr)));
            this->AddOperande(operande(addr,"string", ReadStringSubByteArray(content, addr)));
            QByteArray IDFun_bytearray = ReadSubByteArray(content, addr,4);
            int IDfun = ReadIntegerFromByteArray(0, IDFun_bytearray);
            if (IDfun!=-1){
                std::vector<function>::iterator it = find_function_by_ID(Maker->FunctionsToParse,IDfun);
                if (!std::count(Maker->FunctionsParsed.begin(), Maker->FunctionsParsed.end(), *it)){
                    qDebug() << "Calling CreateMonster function at addr " << hex << it->actual_addr << " ID: "<< hex << it->ID;
                    it->AddInstruction(std::make_shared<CreateMonsters>(it->actual_addr,content,Maker));
                    Maker->FunctionsParsed.push_back(*it);
                }
            }
            this->AddOperande(operande(addr,"int", IDFun_bytearray));
            this->AddOperande(operande(addr,"byte", ReadSubByteArray(content, addr,1)));
            this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));
            this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));
            this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));

    }
};
class OPCode1E : public Instruction
{
    public:
    OPCode1E():Instruction(0x1E,nullptr){}
    OPCode1E(Builder *Maker):Instruction("???",0x1E,Maker){}
    OPCode1E(int &addr, QByteArray &content, Builder *Maker):Instruction("???", 0x1E,Maker){
            addr++;
            this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));
            QByteArray control_byte = ReadSubByteArray(content, addr, 1);
            this->AddOperande(operande(addr,"byte", control_byte));
            this->AddOperande(operande(addr,"string", ReadStringSubByteArray(content, addr)));

    }
};
class OPCode1F : public Instruction
{
    public:
    OPCode1F():Instruction(0x1F,nullptr){}
    OPCode1F(Builder *Maker):Instruction("???",0x1F,Maker){}
    OPCode1F(int &addr, QByteArray &content, Builder *Maker):Instruction("???", 0x1F,Maker){
            addr++;
            this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));
            QByteArray control_byte = ReadSubByteArray(content, addr, 1);
            this->AddOperande(operande(addr,"byte", control_byte));


    }
};
class OPCode22 : public Instruction
{
    public:
    OPCode22():Instruction(0x22,nullptr){}
    OPCode22(Builder *Maker):Instruction("???",0x22,Maker){}
    OPCode22(int &addr, QByteArray &content, Builder *Maker):Instruction("???", 0x22,Maker){
            addr++;
            this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr, 2)));
            fun_140498b70(addr, content, this);
    }


};
class OPCode23 : public Instruction
{
    public:
    OPCode23():Instruction(0x23,nullptr){}
    OPCode23(Builder *Maker):Instruction("???",0x23,Maker){}
    OPCode23(int &addr, QByteArray &content, Builder *Maker):Instruction("???", 0x23,Maker){
            addr++;
            QByteArray control_byte = ReadSubByteArray(content, addr, 1);
            this->AddOperande(operande(addr,"byte", control_byte));

            switch ((unsigned char)control_byte[0])  {
                case 0:
                case 5:
                    this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr, 2)));
                    this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr, 2)));
                    this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr, 2)));
                    this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr, 2)));
                    this->AddOperande(operande(addr,"byte", ReadSubByteArray(content, addr, 1)));
                    break;
                case 1:
                    this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr, 2)));
                    this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr, 2)));
                    this->AddOperande(operande(addr,"byte", ReadSubByteArray(content, addr, 1)));
                    this->AddOperande(operande(addr,"byte", ReadSubByteArray(content, addr, 1)));
                    break;
                case 2:
                    this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr, 2)));
                    this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr, 2)));
                    this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr, 2)));
                    break;
            }
    }


};
class OPCode24 : public Instruction
{
    public:
    OPCode24():Instruction(0x24,nullptr){}
    OPCode24(Builder *Maker):Instruction("???",0x24,Maker){}
    OPCode24(int &addr, QByteArray &content, Builder *Maker):Instruction("???", 0x24,Maker){
            addr++;
            this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr, 2)));
            this->AddOperande(operande(addr,"int", ReadSubByteArray(content, addr, 4)));
            fun_140498b70(addr, content, this);

    }


};
class OPCode25 : public Instruction
{
    public:
    OPCode25():Instruction(0x25,nullptr){}
    OPCode25(Builder *Maker):Instruction("???",0x25,Maker){}
    OPCode25(int &addr, QByteArray &content, Builder *Maker):Instruction("???", 0x25,Maker){
            addr++;
            this->AddOperande(operande(addr,"byte", ReadSubByteArray(content, addr, 1)));
    }


};
class OPCode26 : public Instruction
{
    public:
    OPCode26():Instruction(0x26,nullptr){}
    OPCode26(Builder *Maker):Instruction("???",0x26,Maker){}
    OPCode26(int &addr, QByteArray &content, Builder *Maker):Instruction("???", 0x26,Maker){
            addr++;
    }


};
class OPCode27 : public Instruction
{
    public:
    OPCode27():Instruction(0x27,nullptr){}
    OPCode27(Builder *Maker):Instruction("???",0x27,Maker){}
    OPCode27(int &addr, QByteArray &content, Builder *Maker):Instruction("???", 0x27,Maker){
            addr++;
            this->AddOperande(operande(addr,"string", ReadStringSubByteArray(content, addr)));
            this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr, 2)));
    }


};
class OPCode2F : public Instruction
{
    public:
    OPCode2F():Instruction(0x2F,nullptr){}
    OPCode2F(Builder *Maker):Instruction("???",0x2F,Maker){}
    OPCode2F(int &addr, QByteArray &content, Builder *Maker):Instruction("???", 0x2F,Maker){
            addr++;
            QByteArray control_byte = ReadSubByteArray(content, addr, 1);
            this->AddOperande(operande(addr,"byte", control_byte));
            this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr, 2)));
            switch ((unsigned char)control_byte[0])  {
            case 1:
            case 0:
            case 2:
            case 4:
            case 5:
                this->AddOperande(operande(addr,"string", ReadStringSubByteArray(content, addr)));
                this->AddOperande(operande(addr,"string", ReadStringSubByteArray(content, addr)));
                break;
            case 6:
            case 7:
                this->AddOperande(operande(addr,"int", ReadSubByteArray(content, addr, 4)));
                this->AddOperande(operande(addr,"int", ReadSubByteArray(content, addr, 4)));
                break;
            case 8:

                this->AddOperande(operande(addr,"byte", ReadSubByteArray(content, addr, 1)));
                this->AddOperande(operande(addr,"string", ReadStringSubByteArray(content, addr)));
                this->AddOperande(operande(addr,"string", ReadStringSubByteArray(content, addr)));
                this->AddOperande(operande(addr,"string", ReadStringSubByteArray(content, addr)));
                this->AddOperande(operande(addr,"string", ReadStringSubByteArray(content, addr)));
                this->AddOperande(operande(addr,"string", ReadStringSubByteArray(content, addr)));
                this->AddOperande(operande(addr,"string", ReadStringSubByteArray(content, addr)));
                this->AddOperande(operande(addr,"string", ReadStringSubByteArray(content, addr)));
                this->AddOperande(operande(addr,"string", ReadStringSubByteArray(content, addr)));
                this->AddOperande(operande(addr,"string", ReadStringSubByteArray(content, addr)));
                this->AddOperande(operande(addr,"string", ReadStringSubByteArray(content, addr)));
                this->AddOperande(operande(addr,"string", ReadStringSubByteArray(content, addr)));
                this->AddOperande(operande(addr,"string", ReadStringSubByteArray(content, addr)));
                this->AddOperande(operande(addr,"string", ReadStringSubByteArray(content, addr)));
                this->AddOperande(operande(addr,"string", ReadStringSubByteArray(content, addr)));
                this->AddOperande(operande(addr,"string", ReadStringSubByteArray(content, addr)));
                this->AddOperande(operande(addr,"string", ReadStringSubByteArray(content, addr)));//16
                break;
            case 9:
                this->AddOperande(operande(addr,"byte", ReadSubByteArray(content, addr, 1)));
                break;
            case 0xA:
            case 0xB:
            case 0xC:
                this->AddOperande(operande(addr,"string", ReadStringSubByteArray(content, addr)));
                this->AddOperande(operande(addr,"string", ReadStringSubByteArray(content, addr)));
                break;
            case 0xE:
                this->AddOperande(operande(addr,"int", ReadSubByteArray(content, addr, 4)));
                this->AddOperande(operande(addr,"string", ReadStringSubByteArray(content, addr)));
                this->AddOperande(operande(addr,"string", ReadStringSubByteArray(content, addr)));
                break;
            case 0xF:
                this->AddOperande(operande(addr,"byte", ReadSubByteArray(content, addr, 1)));
                break;
            }

    }


};

class OPCode32 : public Instruction
{
    public:
    OPCode32():Instruction(0x32,nullptr){}
    OPCode32(Builder *Maker):Instruction("???",0x32,Maker){}
    OPCode32(int &addr, QByteArray &content, Builder *Maker):Instruction("???", 0x32,Maker){
            addr++;
            QByteArray control_byte = ReadSubByteArray(content, addr, 1);
            this->AddOperande(operande(addr,"byte", control_byte));

            switch ((unsigned char)control_byte[0])  {
            case '\n':
                this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr, 2)));
                this->AddOperande(operande(addr,"byte", ReadSubByteArray(content, addr, 1)));
                this->AddOperande(operande(addr,"string", ReadStringSubByteArray(content, addr)));

                break;

            case '\v':
                this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr, 2)));
                this->AddOperande(operande(addr,"byte", ReadSubByteArray(content, addr, 1)));
                break;
            case '\f':
                this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr, 2)));
                fun_1403c90e0(addr,content,this, 1);
                this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr, 2)));
                this->AddOperande(operande(addr,"int", ReadSubByteArray(content, addr, 4)));
                fun_1403c90e0(addr,content,this, 0);
                fun_1403c90e0(addr,content,this, 0);
                fun_1403c90e0(addr,content,this, 0);
                fun_1403c90e0(addr,content,this, 0);//0xFF
                this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr, 4)));
                this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr, 4)));
                this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr, 4)));
                fun_1403c90e0(addr,content,this, 0);
                fun_1403c90e0(addr,content,this, 0);
                fun_1403c90e0(addr,content,this, 0);
                this->AddOperande(operande(addr,"byte", ReadSubByteArray(content, addr, 1)));
                //Here we need to use the function fun_1403c90e0 multiple times
                break;
            }
            if (1 < (control_byte[0] - 0xfU)){
                switch((unsigned char)control_byte[0]){
                case '\x11':
                    this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr, 2)));
                    this->AddOperande(operande(addr,"byte", ReadSubByteArray(content, addr, 1)));
                    this->AddOperande(operande(addr,"byte", ReadSubByteArray(content, addr, 1)));
                    break;
                case '\x12':

                    this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr, 2)));
                    this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr, 2)));
                    break;
                case '\x13':
                    this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr, 2)));
                    this->AddOperande(operande(addr,"byte", ReadSubByteArray(content, addr, 1)));
                    this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr, 2)));
                     break;

                default:
                    if ((unsigned char)(control_byte[0] - 0x14U) < 2){
                        this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr, 2)));
                        this->AddOperande(operande(addr,"byte", ReadSubByteArray(content, addr, 1)));
                        this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr, 4)));//3
                        this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr, 4)));
                        this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr, 4)));
                        this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr, 4)));//F
                        this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr, 2)));
                    }
                    break;

                }

            }
            else{
                this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr, 2)));
                this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr, 4)));
                this->AddOperande(operande(addr,"byte", ReadSubByteArray(content, addr, 1)));

            }
            switch((unsigned char)control_byte[0]){
                case '\x16':
                    this->AddOperande(operande(addr,"string", ReadStringSubByteArray(content, addr)));
                    break;
                case '\x17':
                    this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr, 2)));
                    this->AddOperande(operande(addr,"byte", ReadSubByteArray(content, addr, 1)));
                    this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr, 4)));
                    break;
                case '\x18':
                    this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr, 2)));
                    this->AddOperande(operande(addr,"byte", ReadSubByteArray(content, addr, 1)));
                    this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr, 2)));
                    break;
                case '\x19':

                    this->AddOperande(operande(addr,"byte", ReadSubByteArray(content, addr, 1)));
                    this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr, 4)));
                    this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr, 4)));
                    break;
                case '\x1A':
                    this->AddOperande(operande(addr,"string", ReadStringSubByteArray(content, addr)));
                    break;
                case '\x1B':
                    this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr, 2)));
                    break;
                case '\x1C':
                    this->AddOperande(operande(addr,"int", ReadSubByteArray(content, addr, 4)));
                    this->AddOperande(operande(addr,"string", ReadStringSubByteArray(content, addr)));

                    break;
                case '\x1D':
                    this->AddOperande(operande(addr,"int", ReadSubByteArray(content, addr, 4)));
                    break;
                }

          }



};
class OPCode35 : public Instruction
{
    public:
    OPCode35():Instruction(0x35,nullptr){}
    OPCode35(Builder *Maker):Instruction("???",0x35,Maker){}
    OPCode35(int &addr, QByteArray &content, Builder *Maker):Instruction("???", 0x35,Maker){
            addr++;
            QByteArray control_byte = ReadSubByteArray(content, addr, 1);
            this->AddOperande(operande(addr,"byte", control_byte));
            this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));
            this->AddOperande(operande(addr,"int", ReadSubByteArray(content, addr,4)));

    }
};
class OPCode36 : public Instruction
{
    public:
    OPCode36():Instruction(0x36,nullptr){}
    OPCode36(Builder *Maker):Instruction("???",0x36,Maker){}
    OPCode36(int &addr, QByteArray &content, Builder *Maker):Instruction("???", 0x36,Maker){
            addr++;
            QByteArray control_byte = ReadSubByteArray(content, addr, 1);
            this->AddOperande(operande(addr,"byte", control_byte));
            switch ((unsigned char)control_byte[0])  {
                case 0:
                    break;
                case 2:
                    this->AddOperande(operande(addr,"byte", ReadSubByteArray(content, addr,1)));
                    this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));
                    this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));
                    this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));
                    this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));
                    break;
                case 3:
                case 0x14:
                    this->AddOperande(operande(addr,"byte", ReadSubByteArray(content, addr,1)));
                    this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));
                    this->AddOperande(operande(addr,"string", ReadStringSubByteArray(content, addr)));
                    this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));
                    this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));
                    this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));
                    this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));
                    break;
                case 0x4:
                    this->AddOperande(operande(addr,"byte", ReadSubByteArray(content, addr,1)));
                    this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));
                    this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));
                    this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));
                    this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));
                    this->AddOperande(operande(addr,"byte", ReadSubByteArray(content, addr,1)));
                    break;
                case 0x5:
                case 0x16:
                    this->AddOperande(operande(addr,"byte", ReadSubByteArray(content, addr,1)));
                    this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));
                    this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));

                    break;
                case 0x6:
                    this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));
                    this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));

                    break;
                case 0x7:
                case 0x17:
                    this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));

                    break;
                case 0x8:
                    this->AddOperande(operande(addr,"byte", ReadSubByteArray(content, addr,1)));
                    this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));

                    break;
                case 0x9:
                    this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));
                    this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));
                    this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));
                    break;
                case 0xA:
                    this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));//0
                    this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));//4
                    this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));//8->c
                    this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));//c->E
                    this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));//E->10
                    this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));//10->12
                    this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));//12->14
                    this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));//14->16
                    this->AddOperande(operande(addr,"byte", ReadSubByteArray(content, addr,1)));
                    break;
                case 0xB:
                    this->AddOperande(operande(addr,"byte", ReadSubByteArray(content, addr,1)));
                    this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));
                    this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));

                    break;
                case 0xC:
                    this->AddOperande(operande(addr,"byte", ReadSubByteArray(content, addr,1)));
                    this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));
                    this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));
                    this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));
                    this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));

                    break;
                case 0xD:
                    this->AddOperande(operande(addr,"byte", ReadSubByteArray(content, addr,1)));
                    this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));
                    this->AddOperande(operande(addr,"string", ReadStringSubByteArray(content, addr)));
                    this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));
                    this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));
                    this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));
                    this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));

                    break;
                case 0xE:
                    this->AddOperande(operande(addr,"byte", ReadSubByteArray(content, addr,1)));
                    this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));
                    this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));
                    this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));
                    this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));

                    break;
                case 0xF:
                    this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));
                    this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));
                    this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));
                    this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));

                    break;
                case 0x11:
                    this->AddOperande(operande(addr,"byte", ReadSubByteArray(content, addr,1)));
                    this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));
                    this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));
                    this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));
                    this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));
                    this->AddOperande(operande(addr,"byte", ReadSubByteArray(content, addr,1)));
                    break;
                case 0x12:
                    this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));
                    break;
                case 0x13:{
                    this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));

                    int lVar7 = add_operandes(addr, content, this, 3);
                    this->AddOperande(operande(addr,"byte", ReadSubByteArray(content, addr,1)));
                    this->AddOperande(operande(addr,"byte", ReadSubByteArray(content, addr,1)));
                    this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));
                    this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));
                    this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));
                    this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));
                    this->AddOperande(operande(addr,"byte", ReadSubByteArray(content, addr,1)));
                    break;}
                case 0x15:
                case 0x1f:{
                    this->AddOperande(operande(addr,"byte", ReadSubByteArray(content, addr,1)));
                    this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));
                    this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));
                    break;}
                case 0x19:
                case 0x18:{
                    this->AddOperande(operande(addr,"byte", ReadSubByteArray(content, addr,1)));
                    this->AddOperande(operande(addr,"string", ReadStringSubByteArray(content, addr)));
                    this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));
                    this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));
                    this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));
                    break;}
                case 0x1A:{
                    this->AddOperande(operande(addr,"string", ReadStringSubByteArray(content, addr)));
                    this->AddOperande(operande(addr,"byte", ReadSubByteArray(content, addr,1)));
                    this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));//2->6
                    this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));//6->10
                    this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));//10->14 (d)
                    this->AddOperande(operande(addr,"byte", ReadSubByteArray(content, addr,1)));//(e)
                    this->AddOperande(operande(addr,"byte", ReadSubByteArray(content, addr,1)));//(f)
                    this->AddOperande(operande(addr,"byte", ReadSubByteArray(content, addr,1)));//(0x10)
                    break;}
                case 0x1B:{

                    this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));
                    this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));

                    break;}
                case 0x28:
                case 0x29:{

                    this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));
                    this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));
                    this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));

                    break;}
                case 0x2A:
                case 0x2B:{
                    this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));
                    break;}
                case 0x2C:
                case 0x2D:{
                    this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));
                    break;}
                case 0x2E:{
                    this->AddOperande(operande(addr,"byte", ReadSubByteArray(content, addr,1)));
                    break;}
            }
    }
};
class OPCode37 : public Instruction
{
    public:
    OPCode37():Instruction(0x37,nullptr){}
    OPCode37(Builder *Maker):Instruction("???",0x37,Maker){}
    OPCode37(int &addr, QByteArray &content, Builder *Maker):Instruction("???", 0x37,Maker){
            addr++;
            this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));
            this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));
            this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));
            this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));
            this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));
    }
};
class OPCode38 : public Instruction
{
    public:
    OPCode38():Instruction(0x38,nullptr){}
    OPCode38(Builder *Maker):Instruction("???",0x38,Maker){}
    OPCode38(int &addr, QByteArray &content, Builder *Maker):Instruction("???", 0x38,Maker){
            addr++;
            this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));
            this->AddOperande(operande(addr,"byte", ReadSubByteArray(content, addr,1)));
            this->AddOperande(operande(addr,"byte", ReadSubByteArray(content, addr,1)));
            this->AddOperande(operande(addr,"byte", ReadSubByteArray(content, addr,1)));
            this->AddOperande(operande(addr,"string", ReadStringSubByteArray(content, addr)));
    }
};

class OPCode39 : public Instruction
{
    public:
    OPCode39():Instruction(0x39,nullptr){}
    OPCode39(Builder *Maker):Instruction("???",0x39,Maker){}
    OPCode39(int &addr, QByteArray &content, Builder *Maker):Instruction("???", 0x39,Maker){
            addr++;
            this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));
            this->AddOperande(operande(addr,"byte", ReadSubByteArray(content, addr,1)));
            this->AddOperande(operande(addr,"string", ReadStringSubByteArray(content, addr)));
            this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));
            this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));
            this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));
    }
};

class OPCode3A : public Instruction
{
    public:
    OPCode3A():Instruction(0x3A,nullptr){}
    OPCode3A(Builder *Maker):Instruction("???",0x3A,Maker){}
    OPCode3A(int &addr, QByteArray &content, Builder *Maker):Instruction("???", 0x3A,Maker){
            addr++;
            QByteArray control_byte = ReadSubByteArray(content, addr, 1);
            this->AddOperande(operande(addr,"byte", control_byte));
            switch ((unsigned char)control_byte[0])  {
                case 0x0:
                case 0x4:
                    this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));
                    this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));
                    this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));
                    this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));
                    this->AddOperande(operande(addr,"byte", ReadSubByteArray(content, addr, 1)));
                    break;
                case 0x1:
                    this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));
                    this->AddOperande(operande(addr,"byte", ReadSubByteArray(content, addr, 1)));
                    this->AddOperande(operande(addr,"byte", ReadSubByteArray(content, addr, 1)));
                    break;
                case 0x2:
                    this->AddOperande(operande(addr,"byte", ReadSubByteArray(content, addr, 1)));
                    break;
                case 0x3:
                    this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));
                    this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));
                    this->AddOperande(operande(addr,"byte", ReadSubByteArray(content, addr, 1)));
                    break;
                case 0x5:
                    this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));
                    this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));
                    break;
                case 0x6:
                    this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));

                    break;
                case 0x7:

                    break;
                case 0x8:
                    this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));
                    this->AddOperande(operande(addr,"byte", ReadSubByteArray(content, addr, 1)));
                    break;
                case 0x9:
                    this->AddOperande(operande(addr,"byte", ReadSubByteArray(content, addr, 1)));
                    break;
            }


    }
};
class OPCode3B : public Instruction
{
    public:
    OPCode3B():Instruction(0x3B,nullptr){}
    OPCode3B(Builder *Maker):Instruction("???",0x3B,Maker){}
    OPCode3B(int &addr, QByteArray &content, Builder *Maker):Instruction("???", 0x3B,Maker){
            addr++;
            QByteArray control_byte = ReadSubByteArray(content, addr, 1);
            this->AddOperande(operande(addr,"byte", control_byte));

            switch ((unsigned char)control_byte[0])  {
                case 0xFB:
                case 0xFD:
                    this->AddOperande(operande(addr,"byte", ReadSubByteArray(content, addr, 1)));
                    break;
                case 0xFE:
                    this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr, 2)));
                    break;
                case 0xFF:
                    this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr, 4)));
                    this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr, 4)));
                    this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr, 4)));
                    break;
                case 0:
                case 0x32:
                    fun_1403c90e0(addr, content, this, 1);
                    this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr, 4)));
                    fun_1403c90e0(addr, content, this, 1);

                    this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr, 4)));//0->4
                    this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr, 4)));//4->8
                    this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr, 2)));//8->A
                    this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr, 2)));//A->C
                    this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr, 4)));//C->10
                    this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr, 2)));//10->12
                    this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr, 2)));//12->14
                    this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr, 4)));//14->18
                    this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr, 4)));//18->1C

                    this->AddOperande(operande(addr,"string", ReadStringSubByteArray(content, addr)));//x

                    this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr, 2)));//1C+x
                    this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr, 2)));//1E+x
                    this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr, 2)));//20+x
                    this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr, 2)));//22+x
                    this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr, 2)));//24+x
                    this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr, 2)));//26+x
                    this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr, 2)));//28+x
                    this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr, 2)));//2A+x
                    this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr, 4)));//2C+x

                    break;
                case 1:
                case 0x33:
                    fun_1403c90e0(addr, content, this, 1);
                    fun_1403c90e0(addr, content, this, 1);
                    break;
                case 2:
                case 0x34:
                    fun_1403c90e0(addr, content, this, 1);
                    fun_1403c90e0(addr, content, this, 1);
                    break;
                case 3:
                case 0x35:
                    fun_1403c90e0(addr, content, this, 1);
                    this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr, 4)));
                    this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr, 2)));
                    break;
                case 4:
                case 0x36:
                    fun_1403c90e0(addr, content, this, 1);
                    this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr, 4)));
                    this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr, 2)));
                    this->AddOperande(operande(addr,"byte", ReadSubByteArray(content, addr, 1)));
                    break;
                case 5:
                case 0x37://very weird one; caution

                    this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr, 2)));
                    this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr, 2)));

                    break;
                case 6:
                case 7:
                case 8:
                case 9:
                case 10:
                case 0xb:
                case 0xc:
                case 0xd:
                case 0xe:
                case 0xf:
                case 0x10:
                case 0x11:
                case 0x12:
                case 0x13:
                case 0x14:
                case 0x15:
                case 0x16:
                case 0x17:
                case 0x18:
                case 0x19:
                case 0x1a:
                case 0x1b:
                case 0x1c:
                case 0x1d:
                case 0x1e:
                case 0x1f:
                case 0x20:
                case 0x21:
                case 0x22:
                case 0x23:
                case 0x24:
                case 0x25:
                case 0x26:
                case 0x27:
                case 0x28:
                case 0x29:
                case 0x2a:
                case 0x2b:
                case 0x2c:
                case 0x2d:
                case 0x2e:
                case 0x2f:
                case 0x30:
                case 0x31:
                case 0x38:
                case 0x3f:
                case 0x40:
                case 0x41:
                case 0x42:
                case 0x43:
                case 0x44:
                case 0x45:
                case 0x46:
                case 0x47:
                case 0x48:
                case 0x49:
                case 0x4a:
                case 0x4b:
                case 0x4c:
                case 0x4d:
                case 0x4e:
                case 0x4f:
                case 0x50:
                case 0x51:
                case 0x52:
                case 0x53:
                case 0x54:
                case 0x55:
                case 0x56:
                case 0x57:
                case 0x58:
                case 0x59:
                case 0x5a:
                case 0x5b:
                case 0x5c:
                case 0x5d:
                case 0x5e:
                case 0x5f:
                case 0x60:
                case 0x61:
                case 0x62:
                case 99:
                case 0x66:
                case 0x67:
                case 0x68:
                case 0x69:
                case 0x6a:
                case 0x6b:
                case 0x6c:
                case 0x6d:
                case 0x6e:
                case 0x6f:
                case 0x70:
                case 0x71:
                case 0x72:
                case 0x73:
                case 0x74:
                case 0x75:
                case 0x76:
                case 0x77:
                case 0x78:
                case 0x79:
                case 0x7a:
                case 0x7b:
                case 0x7c:
                case 0x7d:
                case 0x7e:
                case 0x7f:
                case 0x80:
                case 0x81:
                case 0x82:
                case 0x83:
                case 0x84:
                case 0x85:
                case 0x86:
                case 0x87:
                case 0x88:
                case 0x89:
                case 0x8a:
                case 0x8b:
                case 0x8c:
                case 0x8d:
                case 0x8e:
                case 0x8f:
                case 0x90:
                case 0x91:
                case 0x92:
                case 0x93:
                case 0x94:
                case 0x95:
                case 0x3D:
                  break;
                case 0x39:
                    this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));
                    break;
                case 0x3A:
                    this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));
                    fun_1403c90e0(addr, content, this, 1);
                    fun_1403c90e0(addr, content, this, 1);
                    fun_1403c90e0(addr, content, this, 1);
                    fun_1403c90e0(addr, content, this, 1);
                    break;
                case 0x3B:
                case 0x3C:
                    this->AddOperande(operande(addr,"int", ReadSubByteArray(content, addr,4)));
                    break;
                case 0x3E:
                    this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));
                    break;
                case 100:
                    this->AddOperande(operande(addr,"int", ReadSubByteArray(content, addr,4)));
                    this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));
                    this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));
                    break;
                case 0x65:
                    this->AddOperande(operande(addr,"int", ReadSubByteArray(content, addr,4)));//0->4
                    this->AddOperande(operande(addr,"int", ReadSubByteArray(content, addr,4)));//4->8
                    this->AddOperande(operande(addr,"int", ReadSubByteArray(content, addr,4)));//8->C
                    this->AddOperande(operande(addr,"int", ReadSubByteArray(content, addr,4)));//C->10
                    this->AddOperande(operande(addr,"int", ReadSubByteArray(content, addr,4)));//10->14
                    this->AddOperande(operande(addr,"int", ReadSubByteArray(content, addr,4)));//14->18
                    this->AddOperande(operande(addr,"int", ReadSubByteArray(content, addr,4)));//18->1C
                    this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));//1C->20
                    this->AddOperande(operande(addr,"int", ReadSubByteArray(content, addr,4)));//20->24
                    this->AddOperande(operande(addr,"int", ReadSubByteArray(content, addr,4)));//24->28
                    this->AddOperande(operande(addr,"int", ReadSubByteArray(content, addr,4)));//28->2C
                    this->AddOperande(operande(addr,"int", ReadSubByteArray(content, addr,4)));//2C->30
                    this->AddOperande(operande(addr,"int", ReadSubByteArray(content, addr,4)));//30->34
                    this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));//34->38
                    this->AddOperande(operande(addr,"int", ReadSubByteArray(content, addr,4)));//38->3C
                    break;
                case 0x96:
                    this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));
                    break;
                default:
                    this->AddOperande(operande(addr,"byte", ReadSubByteArray(content, addr,1)));
                    break;
            }


    }
};
class OPCode3C : public Instruction
{
    public:
    OPCode3C():Instruction(0x3C,nullptr){}
    OPCode3C(Builder *Maker):Instruction("???",0x3C,Maker){}
    OPCode3C(int &addr, QByteArray &content, Builder *Maker):Instruction("???", 0x3C,Maker){
            addr++;
            QByteArray control_byte = ReadSubByteArray(content, addr, 1);
            this->AddOperande(operande(addr,"byte", control_byte));
            this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));
            switch ((unsigned char)control_byte[0])  {
                case 0x1:
                    this->AddOperande(operande(addr,"string", ReadStringSubByteArray(content, addr)));
                    this->AddOperande(operande(addr,"string", ReadStringSubByteArray(content, addr)));
                break;
                case 0x3:
                    this->AddOperande(operande(addr,"string", ReadStringSubByteArray(content, addr)));
                    this->AddOperande(operande(addr,"string", ReadStringSubByteArray(content, addr)));
                    this->AddOperande(operande(addr,"string", ReadStringSubByteArray(content, addr)));
                    this->AddOperande(operande(addr,"string", ReadStringSubByteArray(content, addr)));
                    this->AddOperande(operande(addr,"string", ReadStringSubByteArray(content, addr)));
                    break;
                case 0x4:
                    this->AddOperande(operande(addr,"string", ReadStringSubByteArray(content, addr)));
                    break;

            }


    }
};
class OPCode3D : public Instruction
{
    public:
    OPCode3D():Instruction(0x3D,nullptr){}
    OPCode3D(Builder *Maker):Instruction("???",0x3D,Maker){}
    OPCode3D(int &addr, QByteArray &content, Builder *Maker):Instruction("???", 0x3D,Maker){
            addr++;

            this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));
            this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));
            this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));
            this->AddOperande(operande(addr,"byte", ReadSubByteArray(content, addr,1)));

    }
};
class OPCode3E : public Instruction
{
    public:
    OPCode3E():Instruction(0x3E,nullptr){}
    OPCode3E(Builder *Maker):Instruction("???",0x3E,Maker){}
    OPCode3E(int &addr, QByteArray &content, Builder *Maker):Instruction("???", 0x3E,Maker){
            addr++;
            this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));
            this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));
            this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));
            this->AddOperande(operande(addr,"byte", ReadSubByteArray(content, addr,1)));
    }
};
class OPCode3F : public Instruction
{
    public:
    OPCode3F():Instruction(0x3F,nullptr){}
    OPCode3F(Builder *Maker):Instruction("???",0x3F,Maker){}
    OPCode3F(int &addr, QByteArray &content, Builder *Maker):Instruction("???", 0x3F,Maker){
            addr++;
            this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));
    }
};
class OPCode40 : public Instruction
{
    public:
    OPCode40():Instruction(0x40,nullptr){}
    OPCode40(Builder *Maker):Instruction("???",0x40,Maker){}
    OPCode40(int &addr, QByteArray &content, Builder *Maker):Instruction("???", 0x40,Maker){
            addr++;

            this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));//1
            QByteArray control_short = ReadSubByteArray(content, addr, 2);
            short control = ReadShortFromByteArray(0, control_short);
            this->AddOperande(operande(addr,"short", control_short));//3
            this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));//5
            this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));//9
            this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));//D

            if ((ushort)(control + 0x1feU) < 3){

                this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));//11 -> 15
            }
            this->AddOperande(operande(addr,"byte", ReadSubByteArray(content, addr, 1)));
            this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));
            this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));
            if (control == -0x1fb) {//I'm not sure at all of this behavior, but at the same time I'm not even sure it happens anywhere (lazy)
              while (content[addr] != '\0'){
                this->AddOperande(operande(addr,"byte", ReadSubByteArray(content, addr, 1)));
                this->AddOperande(operande(addr,"byte", ReadSubByteArray(content, addr, 1)));
                this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));
                this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));

              }
            }

    }
};
class OPCode41 : public Instruction
{
    public:
    OPCode41():Instruction(0x41,nullptr){}
    OPCode41(Builder *Maker):Instruction("0x41",0x41,Maker){}
    OPCode41(int &addr, QByteArray &content, Builder *Maker):Instruction("0x41", 0x41,Maker){
            addr++;
            this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));
            this->AddOperande(operande(addr,"byte", ReadSubByteArray(content, addr, 1)));

    }
};
class OPCode42 : public Instruction
{
    public:
    OPCode42():Instruction(0x42,nullptr){}
    OPCode42(Builder *Maker):Instruction("???",0x42,Maker){}
    OPCode42(int &addr, QByteArray &content, Builder *Maker):Instruction("???", 0x42,Maker){
            addr++;
            QByteArray control_byte = ReadSubByteArray(content, addr, 1);
            this->AddOperande(operande(addr,"byte", control_byte));//1
            this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));//2
            this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));//4
            this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));//8
            this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));//C
            this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));//10
            this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));//14
            this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));//18
            qFatal("vérifier 42!!");

    }
};
class OPCode43 : public Instruction
{
    public:
    OPCode43():Instruction(0x43,nullptr){}
    OPCode43(Builder *Maker):Instruction("???",0x43,Maker){}
    OPCode43(int &addr, QByteArray &content, Builder *Maker):Instruction("???", 0x43,Maker){
            addr++;
            QByteArray control_byte = ReadSubByteArray(content, addr, 1);
            this->AddOperande(operande(addr,"byte", control_byte));
            this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));
            qDebug() << " ? :"<< hex <<control_byte[0] << hex << addr;
            switch ((unsigned char)control_byte[0])  {
                case 0xFF:
                    this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));

                    break;
                case 0xFE:
                case 0x05:
                case 0x69:
                case 0x06:
                case 0x6A:
                case 0x10:
                case 0x0B:
                case 0x0C:

                    break;
                default:
                    this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));
                    this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));
                    break;

            }

    }
};
class OPCode44 : public Instruction
{
    public:
    OPCode44():Instruction(0x44,nullptr){}
    OPCode44(Builder *Maker):Instruction("0x44",0x44,Maker){}
    OPCode44(int &addr, QByteArray &content, Builder *Maker):Instruction("0x44", 0x44,Maker){
            addr++;
            this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));
            this->AddOperande(operande(addr,"byte", ReadSubByteArray(content, addr, 1)));
            this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));
            this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));
            this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));
    }
};
class OPCode45 : public Instruction
{
    public:
    OPCode45():Instruction(0x45,nullptr){}
    OPCode45(Builder *Maker):Instruction("0x45",0x45,Maker){}
    OPCode45(int &addr, QByteArray &content, Builder *Maker):Instruction("0x45", 0x45,Maker){
            addr++;
            this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));
            this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));
            this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));
            this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));
            this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));
            this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));
    }
};
class OPCode46 : public Instruction
{
    public:
    OPCode46():Instruction(0x46,nullptr){}
    OPCode46(Builder *Maker):Instruction("0x46",0x46,Maker){}
    OPCode46(int &addr, QByteArray &content, Builder *Maker):Instruction("0x46", 0x46,Maker){

            addr++;
            QByteArray control_byte = ReadSubByteArray(content, addr, 1);
            this->AddOperande(operande(addr,"byte", control_byte));
            this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));
            this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));
            this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));
            switch ((unsigned char)control_byte[0])  {
                case 3:
                    this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));
                    break;
                case 1:
                    this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));
                    this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));
                    this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));
                    break;
            }
    }
};

class OPCode47 : public Instruction
{
    public:
    OPCode47():Instruction(0x47,nullptr){}
    OPCode47(Builder *Maker):Instruction("???",0x47,Maker){}
    OPCode47(int &addr, QByteArray &content, Builder *Maker):Instruction("???", 0x47,Maker){
            addr++;
            this->AddOperande(operande(addr,"byte", ReadSubByteArray(content, addr,1)));
            this->AddOperande(operande(addr,"string", ReadStringSubByteArray(content, addr)));
            this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));
    }
};
class OPCode49 : public Instruction
{
    public:
    OPCode49():Instruction(0x49,nullptr){}
    OPCode49(Builder *Maker):Instruction("0x49",0x49,Maker){}
    OPCode49(int &addr, QByteArray &content, Builder *Maker):Instruction("0x49", 0x49,Maker){

            addr++;
            QByteArray control_byte = ReadSubByteArray(content, addr, 1);
            this->AddOperande(operande(addr,"byte", control_byte));


            switch ((unsigned char)control_byte[0])  {
                case 0:
                case 1:
                case 4:
                case 5:
                case 6:
                case 0x11:
                case '\t':
                case '\n':
                case '\v':
                    this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));
                    break;
                case '\b':
                    this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));
                    this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));
                    this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));
                    break;
                case '\f':
                case 0x17:
                    this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));
                    this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));

                    break;
                case '\r': // pas sûr
                    this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));
                    this->AddOperande(operande(addr,"bytearray", ReadSubByteArray(content, addr,0x18)));
                    break;
                case '\x0e':
                case '\x0f':
                    this->AddOperande(operande(addr,"byte", ReadSubByteArray(content, addr,1)));
                    break;

            }
    }
};
class OPCode50 : public Instruction
{
    public:
    OPCode50():Instruction(0x50,nullptr){}
    OPCode50(Builder *Maker):Instruction("0x50",0x50,Maker){}
    OPCode50(int &addr, QByteArray &content, Builder *Maker):Instruction("0x50", 0x50,Maker){
            addr++;
            fun_1403c90e0(addr, content, this, 1);

    }
};
class OPCode53 : public Instruction
{
    public:
    OPCode53():Instruction(0x53,nullptr){}
    OPCode53(Builder *Maker):Instruction("0x53",0x53,Maker){}
    OPCode53(int &addr, QByteArray &content, Builder *Maker):Instruction("0x53", 0x53,Maker){
            addr++;
            QByteArray control_byte = ReadSubByteArray(content, addr, 1);
            this->AddOperande(operande(addr,"byte", control_byte));
            this->AddOperande(operande(addr,"byte", ReadSubByteArray(content, addr, 1)));


    }
};
class OPCode54 : public Instruction
{
    public:
    OPCode54():Instruction(0x54,nullptr){}
    OPCode54(Builder *Maker):Instruction("???",0x54,Maker){}
    OPCode54(int &addr, QByteArray &content, Builder *Maker):Instruction("???", 0x54,Maker){
            addr++;
            QByteArray control_byte = ReadSubByteArray(content, addr, 1);
            QByteArray control_byte2;
            this->AddOperande(operande(addr,"byte", control_byte));
            switch ((unsigned char)control_byte[0])  {
                case 0x1:
                case 0x3:
                case 0x8:
                case 0x0:
                    this->AddOperande(operande(addr,"int", ReadSubByteArray(content, addr,4)));
                    this->AddOperande(operande(addr,"bytearray", ReadSubByteArray(content, addr,0x20-4)));
                    break;
                case 10:
                case 0xE:
                    this->AddOperande(operande(addr,"byte", ReadSubByteArray(content, addr,1)));
                    break;
                case 0xB:

                    this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,1)));
                    break;
                case 0xD:

                    this->AddOperande(operande(addr,"string", ReadStringSubByteArray(content, addr)));
                    this->AddOperande(operande(addr,"string", ReadStringSubByteArray(content, addr)));
                    this->AddOperande(operande(addr,"int", ReadSubByteArray(content, addr,4)));
                    break;
                case 0x14:

                    this->AddOperande(operande(addr,"string", ReadStringSubByteArray(content, addr)));
                    this->AddOperande(operande(addr,"string", ReadStringSubByteArray(content, addr)));
                    this->AddOperande(operande(addr,"int", ReadSubByteArray(content, addr,4)));
                    this->AddOperande(operande(addr,"int", ReadSubByteArray(content, addr,4)));
                    this->AddOperande(operande(addr,"int", ReadSubByteArray(content, addr,4)));
                    break;
                case 0x15:

                    this->AddOperande(operande(addr,"string", ReadStringSubByteArray(content, addr)));

                    break;
                case 0x17:
                    this->AddOperande(operande(addr,"int", ReadSubByteArray(content, addr,4)));
                    this->AddOperande(operande(addr,"byte", ReadSubByteArray(content, addr,1)));
                    break;
                case 0x24:
                case 0x22:
                case 0x18:
                case 0x28:
                    this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));
                    break;
                case 0x19:
                    this->AddOperande(operande(addr,"string", ReadStringSubByteArray(content, addr)));
                    this->AddOperande(operande(addr,"string", ReadStringSubByteArray(content, addr)));
                    break;
                case 0x1C:
                    this->AddOperande(operande(addr,"string", ReadStringSubByteArray(content, addr)));
                    this->AddOperande(operande(addr,"int", ReadSubByteArray(content, addr,4)));
                    break;
                case 0x21:
                    this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));
                    this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));
                    break;

                case 0x23:
                    this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));
                    this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));
                    this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));
                    this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));
                    break;
                case 0x26:
                    this->AddOperande(operande(addr,"byte", ReadSubByteArray(content, addr,1)));
                    this->AddOperande(operande(addr,"int", ReadSubByteArray(content, addr,4)));
                    this->AddOperande(operande(addr,"int", ReadSubByteArray(content, addr,4)));
                    this->AddOperande(operande(addr,"string", ReadStringSubByteArray(content, addr)));

                    break;

                case 0x29:
                    this->AddOperande(operande(addr,"byte", ReadSubByteArray(content, addr,1)));
                    this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));
                    break;
                case 0x2B:
                    control_byte2 = ReadSubByteArray(content, addr, 1);
                    this->AddOperande(operande(addr,"byte", control_byte2));
                    if ((int)control_byte2[0]==0){
                        this->AddOperande(operande(addr,"string", ReadStringSubByteArray(content, addr)));
                        this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));}
                    break;
                case 0x2C:

                    this->AddOperande(operande(addr,"byte", ReadSubByteArray(content, addr,1)));
                    this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));
                    this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));
                    break;
                case 0x31:
                    control_byte2 = ReadSubByteArray(content, addr, 1);
                    this->AddOperande(operande(addr,"byte", control_byte2));
                    if ((int)control_byte2[0]==0){
                        this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));
                        this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));
                        this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));
                        this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));
                        this->AddOperande(operande(addr,"byte", ReadSubByteArray(content, addr,1)));
                        this->AddOperande(operande(addr,"byte", ReadSubByteArray(content, addr,1)));
                    }
                    else if ((int)control_byte2[0]==1){
                        this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));
                    }
                    break;
            case 0x34:

                this->AddOperande(operande(addr,"int", ReadSubByteArray(content, addr,4)));
                break;

            case 0x35:
                this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));
                this->AddOperande(operande(addr,"int", ReadSubByteArray(content, addr,4)));
                this->AddOperande(operande(addr,"string", ReadStringSubByteArray(content, addr)));
                this->AddOperande(operande(addr,"string", ReadStringSubByteArray(content, addr)));
                this->AddOperande(operande(addr,"string", ReadStringSubByteArray(content, addr)));
                break;
            case 0x37:
                this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));
                this->AddOperande(operande(addr,"byte", ReadSubByteArray(content, addr,1)));
                this->AddOperande(operande(addr,"byte", ReadSubByteArray(content, addr,1)));
                this->AddOperande(operande(addr,"byte", ReadSubByteArray(content, addr,1)));
                break;
            case 0x38:

                this->AddOperande(operande(addr,"byte", ReadSubByteArray(content, addr,1)));
                this->AddOperande(operande(addr,"int", ReadSubByteArray(content, addr,4)));
                break;
            case 0x39:

                this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));
                break;
            case 0x3A:

                this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2))); //0
                this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2))); //2
                this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));//4
                this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));//8
                this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));//C
                this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));//10
                this->AddOperande(operande(addr,"byte", ReadSubByteArray(content, addr,1))); //0x14

                break;
            case 0x3B:

                this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2))); //0
                this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2))); //2
                this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));//4
                this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));//8
                break;

            case 0x3C:

                this->AddOperande(operande(addr,"bytearray", ReadSubByteArray(content, addr,0xA)));
                break;
            case 0x3E:
                this->AddOperande(operande(addr,"byte", ReadSubByteArray(content, addr,1)));
                this->AddOperande(operande(addr,"string", ReadStringSubByteArray(content, addr)));
                this->AddOperande(operande(addr,"int", ReadSubByteArray(content, addr,4)));
                break;

            case 0x40:
            case 0x4B:
                this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));
                break;
            case 0x42:
                this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));
                this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));
                break;
            case 0x44:
                this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));
                this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));
                this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));
                break;
            case 0x45:
            case 0x46:
                this->AddOperande(operande(addr,"int", ReadSubByteArray(content, addr,4)));
                break;

            case 0x47:
                this->AddOperande(operande(addr,"byte", ReadSubByteArray(content, addr,1)));
                this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));
                break;
            case 0x48:
                this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));
                this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));
                break;
            case 0x49:
            case 0x4A:
                this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));
                this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));
                break;
            case 0x4C:
                this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));
                this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));
                this->AddOperande(operande(addr,"byte", ReadSubByteArray(content, addr,1)));
                break;
            case 0x4D:
                this->AddOperande(operande(addr,"int", ReadSubByteArray(content, addr,4)));
                break;
            case 0x4F:
                control_byte2 = ReadSubByteArray(content, addr, 1);
                this->AddOperande(operande(addr,"byte", control_byte2)); //1
                switch((int)control_byte2[0]){
                    case 0:
                        this->AddOperande(operande(addr,"byte", ReadSubByteArray(content, addr, 1)));
                        this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));
                        break;
                    case 1:
                        this->AddOperande(operande(addr,"byte", ReadSubByteArray(content, addr, 1)));

                        break;
                    case 2:
                        this->AddOperande(operande(addr,"byte", ReadSubByteArray(content, addr, 1)));
                        this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));
                        this->AddOperande(operande(addr,"byte", ReadSubByteArray(content, addr, 1)));

                        break;
                    case 3:
                        this->AddOperande(operande(addr,"byte", ReadSubByteArray(content, addr, 1)));
                        break;
                    case 4:
                        this->AddOperande(operande(addr,"byte", ReadSubByteArray(content, addr, 1)));
                        this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));
                        this->AddOperande(operande(addr,"byte", ReadSubByteArray(content, addr, 1)));
                        this->AddOperande(operande(addr,"byte", ReadSubByteArray(content, addr, 1)));
                        break;
                    case '\n':
                        this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2))); //3
                        this->AddOperande(operande(addr,"byte", ReadSubByteArray(content, addr, 1))); //4
                        this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4))); //8
                        this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2))); //10
                        break;
                }
                break;
            case 0x50:
                this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));
                break;
            case 0x51:
                this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));
                this->AddOperande(operande(addr,"string", ReadStringSubByteArray(content, addr)));
                this->AddOperande(operande(addr,"byte", ReadSubByteArray(content, addr, 1)));
                this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));
                break;
            case 0x52:
                this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));
                this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));
                this->AddOperande(operande(addr,"byte", ReadSubByteArray(content, addr, 1)));
                this->AddOperande(operande(addr,"string", ReadStringSubByteArray(content, addr)));


                break;
            case 0x53:
                this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));
                this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));
                break;

            }


    }
};
class OPCode55 : public Instruction
{
    public:
    OPCode55():Instruction(0x55,nullptr){}
    OPCode55(Builder *Maker):Instruction("0x55",0x55,Maker){}
    OPCode55(int &addr, QByteArray &content, Builder *Maker):Instruction("0x55", 0x55,Maker){
            addr++;
            QByteArray control_byte = ReadSubByteArray(content, addr, 1);
            this->AddOperande(operande(addr,"byte", control_byte));
            this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr, 2)));
            this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr, 2)));
            this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr, 2)));
            this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr, 2)));
            this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr, 2)));
            this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr, 2)));
            this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr, 2)));
            this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr, 2)));
            this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr, 2)));
            this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr, 2)));
            this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr, 4)));
            this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr, 4)));
            this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr, 4)));
            this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr, 4)));
            this->AddOperande(operande(addr,"byte", ReadSubByteArray(content, addr, 1)));
            this->AddOperande(operande(addr,"byte", ReadSubByteArray(content, addr, 1)));
            this->AddOperande(operande(addr,"string", ReadStringSubByteArray(content, addr)));
            this->AddOperande(operande(addr,"string", ReadStringSubByteArray(content, addr)));

    }
};
class OPCode5C : public Instruction
{
    public:
    OPCode5C():Instruction(0x5C,nullptr){}
    OPCode5C(Builder *Maker):Instruction("???",0x5C,Maker){}
    OPCode5C(int &addr, QByteArray &content, Builder *Maker):Instruction("???", 0x5C,Maker){
            addr++;
            this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));
            this->AddOperande(operande(addr,"byte", ReadSubByteArray(content, addr,1)));
            this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));
            this->AddOperande(operande(addr,"string", ReadStringSubByteArray(content, addr)));
            this->AddOperande(operande(addr,"byte", ReadSubByteArray(content, addr,1)));
    }
};
class OPCode5E : public Instruction
{
    public:
    OPCode5E():Instruction(0x5E,nullptr){}
    OPCode5E(Builder *Maker):Instruction("???",0x5E,Maker){}
    OPCode5E(int &addr, QByteArray &content, Builder *Maker):Instruction("???", 0x5E,Maker){
            addr++;
            QByteArray control_byte = ReadSubByteArray(content, addr, 1);
            this->AddOperande(operande(addr,"byte", control_byte));
            this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));
            switch((unsigned char)control_byte[0]){
                case 0:
                    this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));
                    this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));
                    this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));
                    this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));
                    this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));
                    this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));
                    this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));
                    break;
                case 1:
                    break;
            }

    }
};
class OPCode63 : public Instruction
{
    public:
    OPCode63():Instruction(0x63,nullptr){}
    OPCode63(Builder *Maker):Instruction("???",0x63,Maker){}
    OPCode63(int &addr, QByteArray &content, Builder *Maker):Instruction("???", 0x63,Maker){
            addr++;
            QByteArray control_short = ReadSubByteArray(content, addr, 2);
            this->AddOperande(operande(addr,"short", control_short));
            short control_shrt = ReadShortFromByteArray(0, control_short);
            QByteArray control_byte = ReadSubByteArray(content, addr, 1);
            this->AddOperande(operande(addr,"byte", control_byte));

    }
};
class OPCode68 : public Instruction
{
    public:
    OPCode68():Instruction(0x68,nullptr){}
    OPCode68(Builder *Maker):Instruction("???",0x68,Maker){}
    OPCode68(int &addr, QByteArray &content, Builder *Maker):Instruction("???", 0x68,Maker){
            addr++;
            QByteArray control_byte = ReadSubByteArray(content, addr, 1);
            this->AddOperande(operande(addr,"byte", control_byte));
            this->AddOperande(operande(addr,"string", ReadStringSubByteArray(content, addr)));
            switch((unsigned char)control_byte[0]){
            case 0:
            case 1:
            case '\v':
                this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));
                break;
            case 2:
            case 3:
            case 4:
                this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));
                this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));
                this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));
                break;
            case '\a':
                this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));
                this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));
                this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));
                this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));
                this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));
                this->AddOperande(operande(addr,"byte", ReadSubByteArray(content, addr, 1)));
                break;
            case '\b':
                this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));
                this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));
                this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));
                this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));
                this->AddOperande(operande(addr,"byte", ReadSubByteArray(content, addr, 1)));
                break;
            case '\x18':
                this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));
                break;
            case '\x1E':
                this->AddOperande(operande(addr,"string", ReadStringSubByteArray(content, addr)));
                this->AddOperande(operande(addr,"string", ReadStringSubByteArray(content, addr)));
                this->AddOperande(operande(addr,"string", ReadStringSubByteArray(content, addr)));
                this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));
                this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));
                this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));
                break;
            }
    }
};
class OPCode6E : public Instruction
{
    public:
    OPCode6E():Instruction(0x6E,nullptr){}
    OPCode6E(Builder *Maker):Instruction("???",0x6E,Maker){}
    OPCode6E(int &addr, QByteArray &content, Builder *Maker):Instruction("???", 0x6E,Maker){
            addr++;
            this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr, 2)));
            this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));
            this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));
            this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));
            this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));
            this->AddOperande(operande(addr,"byte", ReadSubByteArray(content, addr,1)));

    }
};
class OPCode74 : public Instruction
{
    public:
    OPCode74():Instruction(0x74,nullptr){}
    OPCode74(Builder *Maker):Instruction("???",0x74,Maker){}
    OPCode74(int &addr, QByteArray &content, Builder *Maker):Instruction("???", 0x74,Maker){
            addr++;
            this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));
            QByteArray control_byte = ReadSubByteArray(content, addr, 1);
            this->AddOperande(operande(addr,"byte", control_byte));

            switch((unsigned char)control_byte[0]){
                case 0:
                case 0x14:
                    this->AddOperande(operande(addr,"int", ReadSubByteArray(content, addr,4)));
                    break;
                case 2:
                    this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));//0
                    this->AddOperande(operande(addr,"int", ReadSubByteArray(content, addr,4)));//2
                    this->AddOperande(operande(addr,"int", ReadSubByteArray(content, addr,4)));//6
                    this->AddOperande(operande(addr,"int", ReadSubByteArray(content, addr,4)));//A
                    this->AddOperande(operande(addr,"string", ReadStringSubByteArray(content, addr)));
                    break;
                case 3:
                    this->AddOperande(operande(addr,"string", ReadStringSubByteArray(content, addr)));
                    this->AddOperande(operande(addr,"int", ReadSubByteArray(content, addr,4)));
                    this->AddOperande(operande(addr,"int", ReadSubByteArray(content, addr,4)));
                    this->AddOperande(operande(addr,"string", ReadStringSubByteArray(content, addr)));
                    this->AddOperande(operande(addr,"int", ReadSubByteArray(content, addr,4)));

                    break;
            }
    }
};
class OPCode77 : public Instruction
{
    public:
    OPCode77():Instruction(0x77,nullptr){}
    OPCode77(Builder *Maker):Instruction("???",0x77,Maker){}
    OPCode77(int &addr, QByteArray &content, Builder *Maker):Instruction("???", 0x77,Maker){
            addr++;
            this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));

    }
};
class OPCode7E : public Instruction
{
    public:
    OPCode7E():Instruction(0x7E,nullptr){}
    OPCode7E(Builder *Maker):Instruction("???",0x7E,Maker){}
    OPCode7E(int &addr, QByteArray &content, Builder *Maker):Instruction("???", 0x7E,Maker){
            addr++;
            QByteArray control_byte = ReadSubByteArray(content, addr, 1);
            this->AddOperande(operande(addr,"byte", control_byte));

            switch((unsigned char)control_byte[0]){
                case 0:
                case 1:

                    break;
                case 2:
                case 6:
                    this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));
                    break;
                case 3:
                    this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));
                    this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));
                    this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));
                break;
                case 4:
                    this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));
                break;
                case 5:
                    this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));
                break;

            }
    }
};
class OPCode86 : public Instruction
{
    public:
    OPCode86():Instruction(0x86,nullptr){}
    OPCode86(Builder *Maker):Instruction("0x86",0x86,Maker){}
    OPCode86(int &addr, QByteArray &content, Builder *Maker):Instruction("0x86", 0x86,Maker){
            addr++;
            QByteArray control_byte = ReadSubByteArray(content, addr, 1);
            this->AddOperande(operande(addr,"byte", control_byte));
            this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));//2
            this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));//4
            this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));//6
            this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));//8
            this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));//A
            this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));//c
            this->AddOperande(operande(addr,"int", ReadSubByteArray(content, addr, 4)));
            this->AddOperande(operande(addr,"int", ReadSubByteArray(content, addr, 4)));
            this->AddOperande(operande(addr,"string", ReadStringSubByteArray(content, addr)));


    }
};
class OPCode98 : public Instruction
{
    public:
    OPCode98():Instruction(0x98,nullptr){}
    OPCode98(Builder *Maker):Instruction("0x98",0x98,Maker){}
    OPCode98(int &addr, QByteArray &content, Builder *Maker):Instruction("0x98", 0x98,Maker){
            addr++;
            QByteArray control_short = ReadSubByteArray(content, addr, 2);
            this->AddOperande(operande(addr,"short", control_short));
            short control_shrt = ReadShortFromByteArray(0, control_short);
            switch(control_shrt){
             case 1:
             case 2:
                this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));
                break;
             case 3:
                this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));
                this->AddOperande(operande(addr,"byte", ReadSubByteArray(content, addr,1)));
                break;
             case 0x3E8:
             case 0x3E9:
                this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr,4)));
                this->AddOperande(operande(addr,"byte", ReadSubByteArray(content, addr,1)));
                break;
            }
            //2
            this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));//4
            this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));//6
            this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));//8
            this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));//A
            this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));//c
            this->AddOperande(operande(addr,"int", ReadSubByteArray(content, addr, 4)));
            this->AddOperande(operande(addr,"int", ReadSubByteArray(content, addr, 4)));
            this->AddOperande(operande(addr,"string", ReadStringSubByteArray(content, addr)));


    }
};
class OPCodeAC : public Instruction
{
    public:
    OPCodeAC():Instruction(0xAC,nullptr){}
    OPCodeAC(Builder *Maker):Instruction("0xAC",0xAC,Maker){}
    OPCodeAC(int &addr, QByteArray &content, Builder *Maker):Instruction("0xAC", 0xAC,Maker){
            addr++;
            QByteArray control_byte = ReadSubByteArray(content, addr, 1);
            this->AddOperande(operande(addr,"byte", control_byte));
            switch((unsigned char)control_byte[0]){
                case 0x0:
                    this->AddOperande(operande(addr,"byte", ReadSubByteArray(content, addr, 1)));
                    break;
                case 0x2:
                case 0x1:
                    //here we're calling a function that was defined at the beginning
                    this->AddOperande(operande(addr,"int", ReadSubByteArray(content, addr,4)));
                    break;
                case 0x5:
                case '\a':
                case '\t':
                    //here we're calling a function that was defined at the beginning
                    this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr,2)));
                    break;
            }

    }
};
class OPCodeC4 : public Instruction
{
    public:
    OPCodeC4():Instruction(0xC4,nullptr){}
    OPCodeC4(Builder *Maker):Instruction("0xC4",0xC4,Maker){}
    OPCodeC4(int &addr, QByteArray &content, Builder *Maker):Instruction("0xC4", 0xC4,Maker){
            addr++;
            QByteArray control_byte = ReadSubByteArray(content, addr, 1);
            this->AddOperande(operande(addr,"byte", control_byte));
            switch((unsigned char)control_byte[0]){
                case 0x0:

                    break;
                case 0x1:
                    this->AddOperande(operande(addr,"byte", ReadSubByteArray(content, addr, 1)));
                    break;
                case 0x2:
                    this->AddOperande(operande(addr,"byte", ReadSubByteArray(content, addr, 1)));
                    this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr, 2)));
                    break;
                case 0x3:
                    this->AddOperande(operande(addr,"short", ReadSubByteArray(content, addr, 2)));
                    break;
                case 0x4:
                    this->AddOperande(operande(addr,"byte", ReadSubByteArray(content, addr, 1)));
                    fun_1403c90e0(addr, content, this, 1);
                    this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr, 4)));
                    this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr, 4)));
                    this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr, 4)));
                    this->AddOperande(operande(addr,"float", ReadSubByteArray(content, addr, 4)));
                    break;
                case 0x5:
                    this->AddOperande(operande(addr,"byte", ReadSubByteArray(content, addr, 1)));
                    this->AddOperande(operande(addr,"byte", ReadSubByteArray(content, addr, 1)));
                    break;

            }

    }
};
class OPCodeCA : public Instruction
{
    public:
    OPCodeCA():Instruction(0xCA,nullptr){}
    OPCodeCA(Builder *Maker):Instruction("0xCA",0xCA,Maker){}
    OPCodeCA(int &addr, QByteArray &content, Builder *Maker):Instruction("0xCA", 0xCA,Maker){
            addr++;
            QByteArray control_byte = ReadSubByteArray(content, addr, 1);
            this->AddOperande(operande(addr,"byte", control_byte));
            switch((unsigned char)control_byte[0]){
                case 0x0:
                    this->AddOperande(operande(addr,"int", ReadSubByteArray(content, addr, 4)));
                    break;

            }

    }
};
class CS3TranslationFile : public TranslationFile
{
    public:
    CS3TranslationFile():TranslationFile(){}

};
class CS3Builder : public Builder
{
    public:
    CS3Builder(){

    }
    std::shared_ptr<Instruction> CreateInstructionFromXLSX(int row){
        return std::make_shared<OPCode0>();
    }
    std::shared_ptr<Instruction> CreateInstructionFromDAT(int &addr, QByteArray &dat_content){

        int OP = (dat_content[addr]&0xFF);
        qDebug() << "OP CODE ???: " << hex << OP << "at addr: " << hex << addr;
        switch(OP){
            case 0x00: return std::make_shared<OPCode0>(addr,dat_content,this);
            case 0x01: return std::make_shared<OPCode1>(addr,dat_content,this);
            case 0x02: return std::make_shared<OPCode2>(addr,dat_content,this);
            case 0x03: return std::make_shared<OPCode3>(addr,dat_content,this);
            case 0x05: return std::make_shared<OPCode5>(addr,dat_content,this);
            case 0x0C: return std::make_shared<OPCodeC>(addr,dat_content,this);
            case 0x10: return std::make_shared<OPCode10>(addr,dat_content,this);
            case 0x11: return std::make_shared<OPCode11>(addr,dat_content,this);
            case 0x13: return std::make_shared<OPCode13>(addr,dat_content,this);
            case 0x14: return std::make_shared<OPCode14>(addr,dat_content,this);
            case 0x16: return std::make_shared<OPCode16>(addr,dat_content,this);
            case 0x18: return std::make_shared<OPCode18>(addr,dat_content,this);
            case 0x1D: return std::make_shared<OPCode1D>(addr,dat_content,this);
            case 0x1E: return std::make_shared<OPCode1E>(addr,dat_content,this);
            case 0x1F: return std::make_shared<OPCode1F>(addr,dat_content,this);
            case 0x22: return std::make_shared<OPCode22>(addr,dat_content,this);
            case 0x23: return std::make_shared<OPCode23>(addr,dat_content,this);
            case 0x24: return std::make_shared<OPCode24>(addr,dat_content,this);
            case 0x25: return std::make_shared<OPCode25>(addr,dat_content,this);
            case 0x26: return std::make_shared<OPCode26>(addr,dat_content,this);
            case 0x27: return std::make_shared<OPCode27>(addr,dat_content,this);
            case 0x2F: return std::make_shared<OPCode2F>(addr,dat_content,this);
            case 0x32: return std::make_shared<OPCode32>(addr,dat_content,this);
            case 0x35: return std::make_shared<OPCode35>(addr,dat_content,this);
            case 0x36: return std::make_shared<OPCode36>(addr,dat_content,this);
            case 0x37: return std::make_shared<OPCode37>(addr,dat_content,this);
            case 0x38: return std::make_shared<OPCode38>(addr,dat_content,this);
            case 0x39: return std::make_shared<OPCode39>(addr,dat_content,this);
            case 0x3A: return std::make_shared<OPCode3A>(addr,dat_content,this);
            case 0x3B: return std::make_shared<OPCode3B>(addr,dat_content,this);
            case 0x3C: return std::make_shared<OPCode3C>(addr,dat_content,this);
            case 0x3D: return std::make_shared<OPCode3D>(addr,dat_content,this);
            case 0x3E: return std::make_shared<OPCode3E>(addr,dat_content,this);
            case 0x3F: return std::make_shared<OPCode3F>(addr,dat_content,this);
            case 0x40: return std::make_shared<OPCode40>(addr,dat_content,this);
            case 0x41: return std::make_shared<OPCode41>(addr,dat_content,this);
            case 0x42: return std::make_shared<OPCode42>(addr,dat_content,this);
            case 0x43: return std::make_shared<OPCode43>(addr,dat_content,this);
            case 0x44: return std::make_shared<OPCode44>(addr,dat_content,this);
            case 0x45: return std::make_shared<OPCode45>(addr,dat_content,this);
            case 0x46: return std::make_shared<OPCode46>(addr,dat_content,this);
            case 0x47: return std::make_shared<OPCode47>(addr,dat_content,this);
            case 0x49: return std::make_shared<OPCode49>(addr,dat_content,this);
            case 0x50: return std::make_shared<OPCode50>(addr,dat_content,this);
            case 0x53: return std::make_shared<OPCode53>(addr,dat_content,this);
            case 0x54: return std::make_shared<OPCode54>(addr,dat_content,this);
            case 0x55: return std::make_shared<OPCode55>(addr,dat_content,this);
            case 0x5C: return std::make_shared<OPCode5C>(addr,dat_content,this);
            case 0x5E: return std::make_shared<OPCode5E>(addr,dat_content,this);
            case 0x63: return std::make_shared<OPCode63>(addr,dat_content,this);
            case 0x68: return std::make_shared<OPCode68>(addr,dat_content,this);
            case 0x6E: return std::make_shared<OPCode6E>(addr,dat_content,this);
            case 0x74: return std::make_shared<OPCode74>(addr,dat_content,this);
            case 0x77: return std::make_shared<OPCode77>(addr,dat_content,this);
            case 0x7E: return std::make_shared<OPCode7E>(addr,dat_content,this);
            case 0x86: return std::make_shared<OPCode86>(addr,dat_content,this);
            case 0x98: return std::make_shared<OPCode98>(addr,dat_content,this);
            case 0xAC: return std::make_shared<OPCodeAC>(addr,dat_content,this);
            case 0xC4: return std::make_shared<OPCodeC4>(addr,dat_content,this);
            case 0xCA: return std::make_shared<OPCodeCA>(addr,dat_content,this);
            case 256: return std::make_shared<CreateMonsters>(addr,dat_content,this);
            default:
            std::stringstream stream;
            stream << "L'OP code " << std::hex << OP << " n'est pas défini !!";
            std::string result( stream.str() );

            qFatal(result.c_str());

            return std::shared_ptr<Instruction>();
        }

    }
    bool CreateHeaderFromDAT(QByteArray &dat_content){
        //Header structure:
        //The first 0x20 I'd say is probably the position of the name of the file; which shouldn't change
        //The second 0x20 probably doesn't change too.
        //The next integer is the position of the first pointer
        //The 4th: probably the length in bytes of the pointer section
        //The fifth: probably the position of the "names positions" section (right after the pointer section)
        //The sixth: the number of functions inside the file
        //the seventh: the position one byte after the 0x00 separator at the end of the functions section
        //the eighth: 0xABCDEF00 => seems to always be there (no idea why)
        //Then the name of the file
        //Then the pointer section
        //Then the "names positions" section
        //Then the functions section
        //Done; here the function objects holds the number of functions, the addr, name positions
        //everything else can be deduced
        int nb_functions = dat_content[0x14];
        int position = 0x20;
        QString filename = ReadStringFromByteArray(position, dat_content);
        SceneName = filename;
        int start_offset_area = 0x20 + filename.length()+1;
        for (int idx_fun = 0; idx_fun < nb_functions; idx_fun++){
            position = start_offset_area + 4*idx_fun;
            int addr = ReadIntegerFromByteArray(position, dat_content);
            position = start_offset_area + 4*nb_functions + 2 * idx_fun;
            short name_pos = ReadShortFromByteArray(position, dat_content);
            int name_pos_int = name_pos;
            QString function_name = ReadStringFromByteArray(name_pos_int, dat_content);
            FunctionsToParse.push_back(function(idx_fun,function_name,name_pos,addr));
        }
        return true;
    }
    bool CreateHeaderFromXLSX(){
        return true;
    }
};


#endif // CS3INSTRUCTIONSSET_H
