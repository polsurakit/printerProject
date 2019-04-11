//
//  HTTPRequest
//

#include <iostream>
#include <fstream>
#include "HTTPRequest.hpp"

using namespace std;
int main(int argc, const char * argv[])
{
    std::string url = "http://0.0.0.0:5000/";
    std::string method = "GET";
    std::string arguments;
    std::string output;

    
    cout << "url " << url << endl << "method " << method << "\nargument" << arguments << "\noutput " << output << endl;

    try
    {
        http::Request request(url);

        http::Response response = request.send(method, arguments, {
            "Content-Type: application/x-www-form-urlencoded",
            "User-Agent: runscope/0.1"
        });

        if (!output.empty())
        {
            std::ofstream outfile(output, std::ofstream::binary);
            outfile.write(reinterpret_cast<const char*>(response.body.data()),
                          static_cast<std::streamsize>(response.body.size()));
        }
        else{
            
            for(int i = 0 ; i < response.body.size() ; i++){
                cout << response.body[i] ;
            }
            cout << endl;
        }
            //std::cout << response.body.data() << std::endl;
            // cout << "11111111111111" << endl;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Request failed, error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
