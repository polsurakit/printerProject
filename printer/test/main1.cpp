//
//  HTTPRequest
//

#include <iostream>
#include <fstream>
#include "HTTPRequest.hpp"

using namespace std;
int main(int argc, const char * argv[])
{
    std::string url;
    std::string method = "GET";
    std::string arguments;
    std::string output;

    for (int i = 1; i < argc; ++i)
    {
        if (std::string(argv[i]) == "--help")
        {
            std::cout << "test --url <url> [--method <method>] [--arguments <arguments>] [--output <output>]" << std::endl;
            return EXIT_SUCCESS;
        }
        else if (std::string(argv[i]) == "--url")
        {
            if (++i < argc) url = argv[i];
        }
        else if (std::string(argv[i]) == "--method")
        {
            if (++i < argc) method = argv[i];
        }
        else if (std::string(argv[i]) == "--arguments")
        {
            if (++i < argc) arguments = argv[i];
        }
        else if (std::string(argv[i]) == "--output")
        {
            if (++i < argc) output = argv[i];
        }
    }
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
            string body = response.body.data();
            for(int i = 0 ; i < body.length ; i++){
                cout << response.body.data()[i] << endl;
            }
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
