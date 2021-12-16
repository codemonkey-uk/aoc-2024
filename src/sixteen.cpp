// sixteen.cpp

#include <iostream>

#include "util.h"

using namespace std;

class EncodedBitstream
{
    string data;
    
    public:
    EncodedBitstream(char in):data(1,in){}
    EncodedBitstream(const char* in):data(in){}
    EncodedBitstream(string& in)
        : data(in)
    { }
    
    size_t size() const {return data.size()*4;}
    
    bool get(int i)const{
        assert( i<(int)size() );
        char c = data[i/4];
        int n = c<='9' ? c-'0' : c-'0'+9;
        return n & 8>>(i%4);
    }
    bool operator[](int i)const{
        return get(i);
    }
    
    string ToString() const 
    {
        string result;
        result.reserve(size());
        for(size_t i=0;i!=size();++i)
            result.push_back( get(i) ? '1' : '0' );
        return result;
    }
    
    //the first three bits encode the packet version
    int Version(int& i) const
    {
        int result = (get(i) << 2) | (get(i+1) << 1) | get(i+2);
        i+=3;
        return result;
    }
    
    int Type(int& i) const
    {
        int result = (get(i) << 2) | (get(i+1) << 1) | get(i+2);
        i+=3;
        return result;
    }
    
    int ReadLiteral(int& i) const
    {
        size_t result = 0;
        bool continues;
        do 
        {
            continues = get(i++);
            result = (result << 1) | get(i++);
            result = (result << 1) | get(i++);
            result = (result << 1) | get(i++);
            result = (result << 1) | get(i++);
        }while(continues);
        return result;
    }
    
    void ReadOperator(
        int& i, 
        vector<int>* packet_versions=nullptr,
        vector<int>* literal_values=nullptr
    ) const
    {
        static string indent;
        indent.push_back(' ');
        bool length_type = get(i++);
        int sub_packets = 0;
        int bit_len = 0;
        if (length_type==0)
        {
            // the next 15 bits are a number that represents 
            // the total length in bits of the sub-packets 
            // contained by this packet.
            for (int x=0;x!=15;++x)
                bit_len = (bit_len <<1) | get(i++);
            if (debug) cout << indent << "bit_len = " << bit_len << endl;
        }
        else
        {
            // then the next 11 bits are a number that represents 
            // the number of sub-packets immediately contained by this packet.
            for (int x=0;x!=11;++x)
                sub_packets = (sub_packets <<1) | get(i++);
            if (debug) cout << indent << "sub_packets = " << sub_packets << endl;
        }
        
        bool done = false;
        int start_bit = i;
        int packet_count = 0;
        do{
            
            int v = Version(i);
            if (debug) cout << indent << "V=" << v << endl;
            if (packet_versions) packet_versions->push_back(v);
            
            int t = Type(i);
            if (t==4)
            {
                int r = ReadLiteral(i);
                if (debug) cout << indent << "L=" << r << endl;
                if (literal_values) literal_values->push_back(r);
                packet_count++;
            }
            else
            {
                if (debug) cout << indent << "T=" << t << endl;
                ReadOperator(
                    i, 
                    packet_versions,
                    literal_values
                );
                packet_count++;
            }
        
            if (bit_len>0 && i-start_bit == bit_len)
                done = true;
            if (sub_packets>0 && packet_count == sub_packets)
                done = true;
                
        }while(!done);
        
        indent.pop_back();
    }
    
    void Read(
        vector<int>* packet_versions=nullptr,
        vector<int>* literal_values=nullptr
    ) const
    {
        int i=0;
        int v = Version(i);
        int t = Type(i);
        if (packet_versions) packet_versions->push_back(v);
        if (t==4)
        {
            int n = ReadLiteral(i);
            if (literal_values) literal_values->push_back(n);
        }
        else
        {
            ReadOperator(
                i, 
                packet_versions,
                literal_values
            );
        }
    }
};

void Test1()
{
    cout << "Test 1" << endl;
    int i=0;
    EncodedBitstream op_example("38006F45291200");
    assert( op_example.Version(i)==1 );
    assert( op_example.Type(i)==6 );

    vector<int> literal_values;
    op_example.ReadOperator(i, nullptr, &literal_values);
    assert(literal_values.size()==2);
    assert(literal_values[0]==10);
    assert(literal_values[1]==20);
}

void Test2()
{
    cout << "Test 2" << endl;
    int i=0;
    EncodedBitstream op_example2("EE00D40C823060");
    assert( op_example2.Version(i)==7 );
    assert( op_example2.Type(i)==3 );

    vector<int> literal_values;
    op_example2.ReadOperator(i, nullptr, &literal_values);
    assert(literal_values.size()==3);
    assert(literal_values[0]==1);
    assert(literal_values[1]==2);
    assert(literal_values[2]==3);
}

void TestN(const char* str, int expected=0)
{
    cout << "Testing " << str << endl;

    EncodedBitstream test(str);
    vector<int> versions;
    test.Read(&versions);
    
    int total = 0;
    for(int i : versions) total+=i;
    
    if (expected==0) cout << total << endl;
    else assert(total==expected);
}



void Sixteen()
{
    // some tests
    assert( EncodedBitstream("0").ToString()=="0000" );
    assert( EncodedBitstream("1").ToString()=="0001" );
    assert( EncodedBitstream("2").ToString()=="0010" );
    assert( EncodedBitstream("9").ToString()=="1001" );    
    assert( EncodedBitstream("A").ToString()=="1010" );
    assert( EncodedBitstream("F").ToString()=="1111" );

    EncodedBitstream example("D2FE28");
    assert( example.ToString()=="110100101111111000101000" );
    
    int i = 0;
    assert( example.Version(i)==6 );
    assert( example.Type(i)==4 );
    assert( example.ReadLiteral(i)==2021 );
    
    Test1();
    Test2();
    TestN("8A004A801A8002F478",16);
    TestN("620080001611562C8802118E34",12);
    TestN("C0015000016115A2E0802F182340",23);
    TestN("A0016C880162017C3686B18A3D4780",31);
    
    TestN("220D790065B2745FF004672D99A34E5B33439D96CEC80373C0068663101A98C406A5E7395DC1804678BF25A4093BFBDB886CA6E11FDE6D93D16A100325E5597A118F6640600ACF7274E6A5829B00526C167F9C089F15973C4002AA4B22E800FDCFD72B9351359601300424B8C9A00BCBC8EE069802D2D0B945002AB2D7D583E3F00016B05E0E9802BA00B4F29CD4E961491CCB44C6008E80273C393C333F92020134B003530004221347F83A200D47F89913A66FB6620016E24A007853BE5E944297AB64E66D6669FCEA0112AE06009CAA57006A0200EC258FB0440010A8A716A321009DE200D44C8E31F00010887B146188803317A3FC5F30056C0150004321244E88C000874468A91D2291802B25EB875802B28D13550030056C0169FB5B7ECE2C6B2EF3296D6FD5F54858015B8D730BB24E32569049009BF801980803B05A3B41F1007625C1C821256D7C848025DE0040E5016717247E18001BAC37930E9FA6AE3B358B5D4A7A6EA200D4E463EA364EDE9F852FF1B9C8731869300BE684649F6446E584E61DE61CD4021998DB4C334E72B78BA49C126722B4E009C6295F879002093EF32A64C018ECDFAF605989D4BA7B396D9B0C200C9F0017C98C72FD2C8932B7EE0EA6ADB0F1006C8010E89B15A2A90021713610C202004263E46D82AC06498017C6E007901542C04F9A0128880449A8014403AA38014C030B08012C0269A8018E007A801620058003C64009810010722EC8010ECFFF9AAC32373F6583007A48CA587E55367227A40118C2AC004AE79FE77E28C007F4E42500D10096779D728EB1066B57F698C802139708B004A5C5E5C44C01698D490E800B584F09C8049593A6C66C017100721647E8E0200CC6985F11E634EA6008CB207002593785497652008065992443E7872714");

}