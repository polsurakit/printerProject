
    // try
    // {
    //     string url = "http://0.0.0.0:5000/";
    //     std::string method = "GET";
    //     std::string arguments;
    //     std::string output;

    //     http::Request request(url);

    //     http::Response response = request.send(method, arguments, {
    //         "Content-Type: application/x-www-form-urlencoded",
    //         "User-Agent: runscope/0.1"
    //     });

    //     if (!output.empty())
    //     {
    //         std::ofstream outfile(output, std::ofstream::binary);
    //         outfile.write(reinterpret_cast<const char*>(response.body.data()),
    //                       static_cast<std::streamsize>(response.body.size()));
    //     }
    //     else{
    //         vector<string> vs(3);
    //         int idx = 0;
            // for(unsigned int i = 0 ; i < response.body.size() ; i++){
            //     if(response.body[i]==' '){
            //         idx++;
            //         continue;
            //     }
            //     vs[idx] += response.body[i];
            // }
    //         //have to change basis in the section
    //         //and assign to printer's x,y,theta
    //         double retx = stod(vs[0]);
    //         double rety = stod(vs[1]);
    //         double rettheta = stod(vs[2]);
    //         x = retx;
    //         y = rety;
    //         theta = rettheta;
    //         result.push_back(retx);
    //         result.push_back(rety);
    //         result.push_back(rettheta);
    //     }

    //     std::cout << response.body.data() << std::endl;
    //     return result;
    // }
    // catch (const std::exception& e)
    // {
    //     std::cerr << "Request failed, error: " << e.what() << std::endl;
    //     //return EXIT_FAILURE;
    // }
