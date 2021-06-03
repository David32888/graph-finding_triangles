#include <iostream>
#include "input.h"
#include <string>
#include <vector>
#include <fstream>

using vertexes_t = std::vector<std::string>;
using edges_t = std::vector<std::pair<size_t, size_t>>;
struct graph_t 
{
    vertexes_t vertexes;
    edges_t edges;
};
using matrix_t = std::vector<std::vector<int>>;

bool is_space(char ch)
{
    return ch == ' ' || ch == '\f' || ch == '\r' || ch == '\t' || ch == '\v'  || ch == '\n';
}

bool is_digit(char ch)
{
    return ch >= '0' && ch <= '9';
}

bool is_alpha(char ch)
{
    return (ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z');
}

void assert_file(const char* str)
{
    struct stat sb;
    stat(str, &sb);
    if ((sb.st_mode & S_IFMT) != S_IFREG)
    {
        std::cerr << str << ": is not file\n";
        exit(0);
    }
}

bool is_endl(std::istream& in)
{
    while (!in.eof() && is_space(in.peek()) && in.peek() != '\n')
    {
        in.seekg(1, std::istream::ios_base::cur);
    }
    return in.peek() == '\n' || in.eof();
}

inline void reprint(const std::string& str)
{
    printf("\e[31;9m\e[%ldD%s\e[m", str.size(), str.c_str());
    std::flush(std::cout);
}

char get_string(std::string &str, std::istream& in = std::cin)
{
    str.clear();
    cbreak();
    char ch = 0;
    while (!is_space(ch))
    {
        in.get(ch);
        if (is_alpha(ch) || is_digit(ch))
        {
            write(1, &ch, 1);
            str += ch;
        }
        if (ch == 127) //BECKSPACE ASCII = 127
        {
            if (!str.empty())
            {
                str.pop_back();
                write(1, "\b \b", 4);
            }
        }
    }
    normal();
    return ch;
}

static bool operator==(std::pair<size_t, size_t> p1, std::pair<size_t, size_t> p2)
{
    return (p1.first == p2.first && p1.second == p2.second) || (p1.second == p2.first && p1.first == p2.second);
}

template <typename T>
size_t vec_find(const std::vector<T> &vec, const T& t)
{
    for (size_t i = 0; i < vec.size(); ++i)
    {
        if (vec[i] == t)
        {
            return i + 1;
        }
    }
    return 0;
}

vertexes_t input_vertexes()
{
    vertexes_t V;
    std::cout << "V = {" << std::flush;
    for(std::string vertex; get_string(vertex) != '\n' || !vertex.empty();)
    { 
        if (!vertex.empty())
        {
            if (vec_find(V, vertex))
            {
                reprint(vertex);
                std::cout << " " << std::flush;
                continue;
            }
            V.push_back(vertex);
            std::cout << ", " << std::flush;
        }
    }
    std::cout << "Ø}" << std::endl;
    return V;
}

edges_t input_edges(const vertexes_t& vertexes)
{
    edges_t E;
    std::cout << "E = {" << std::flush;
    for (size_t max_size_E = vertexes.size() * (vertexes.size() - 1) / 2; E.size() < max_size_E;)
    {
        std::string vertex1, vertex2;
        std::pair<size_t, size_t> edge;
        std::cout << "{" << std::flush;
        while(get_string(vertex1) != '\n' && vertex1.empty());
        if (vertex1.empty())
        {
            std::cout << "\b \b" << std::flush;
            break;
        }
        if (!(edge.first = vec_find(vertexes, vertex1)))
        {
            std::cout << " , Ø} " << std::flush;
            reprint(std::string("{" + vertex1 + ", Ø} "));
            continue;
        }
        std::cout << ", " << std::flush;
        while(get_string(vertex2) != '\n' && vertex2.empty());
        if (vertex2.empty())
        {
            std::cout << " Ø} " << std::flush;
            reprint(std::string("{" + vertex1 + ", Ø} "));
            continue;
        }
        if (vertex1 == vertex2 || !(edge.second = vec_find(vertexes, vertex2)) || vec_find(E, edge))
        {
            std::cout << "} ";
            reprint(std::string("{" + vertex1 + ", " + vertex2 + "} "));
            continue;
        } 
        std::cout << "}, " << std::flush; 
        E.push_back(edge);
    }
    std::cout << "Ø}" << std::endl;
    return E;
}

graph_t input_graph()
{
    graph_t graph;
    graph.vertexes = input_vertexes();
    graph.edges = input_edges(graph.vertexes);
    return graph;
}

graph_t input_graph(std::istream& Input, bool checking)
{
    graph_t graph;
    while (!is_endl(Input))
    {
        std::string vertex;
        Input >> vertex;
        if (!checking || !vec_find(graph.vertexes, vertex))
        {
            graph.vertexes.push_back(vertex);
        }
    }
    while (!Input.eof())
    {
        std::pair<size_t, size_t> edge;
        Input >> edge.first;
        if (edge.first <= graph.vertexes.size())
        {
            while (!is_endl(Input))
            {
                Input >> edge.second;
                if (edge.second <= graph.vertexes.size())
                {
                    graph.edges.push_back(edge);
                }
            }
        }
    }
    return graph;
}

matrix_t graph_to_matrix(const graph_t& graph)
{
    matrix_t matrix(graph.vertexes.size(), std::vector<int>(graph.vertexes.size(), -1));
    for (size_t i = 0; i < graph.vertexes.size(); ++i)
    {
        matrix[i][i] = i;
    }
    for (size_t k = 0; k < graph.edges.size(); ++k)
    {
        size_t i = graph.edges[k].first - 1;
        size_t j = graph.edges[k].second - 1;
        if (i > j)
        {
            std::swap(i, j);
        }
        matrix[i][j] = matrix[j][i] = matrix[i][i];
        matrix[i][i] = j;
    }
    return matrix;
}

void print_matrix(const matrix_t& matrix)
{
    for (size_t i = 0; i < matrix.size(); ++i)
    {
        for (size_t j = 0; j < matrix[i].size(); ++j)
        {
            std::cout << "\e[m" << std::flush;
            if (matrix[i][j] == -1)
            {
                std::cout << "\e[34m" << std::flush;
            }
            if (i == j)
            {
                std::cout << "\e[33m" << std::flush;
            }
            std::cout.width(2);
            std::cout << matrix[i][j];
        }
        std::cout << std::endl;
    }
    std::cout << "\e[m" << std::flush;
}

std::vector<std::vector<size_t>> triangles_of_graph(const matrix_t& matrix)
{
    std::vector<std::vector<size_t>> triangls;
    for (size_t i = 0; i < matrix.size(); ++i)
    {
        for (size_t j = matrix[i][i]; j != i; j = matrix[i][j])
        {
            for (size_t k = matrix[j][j]; k != j; k = matrix[k][j])
            {
                if (matrix[k][i] >= 0)
                {
                    triangls.push_back(std::vector<size_t>{i, j, k});
                }
            }
        }
    }
    return triangls;
}

int main(int argc, char* argv[])
{
    graph_t graph;
    std::vector<std::vector<size_t>> triangles;
    if (argc > 1)
    {
        int index = 1;
        bool checking = true;
        if (std::string(argv[index]) == "-no_checking")
        {
            ++index;
            checking = false;
        }
        if (argc <= index)
        {
            std::cerr << argv[0] << ": no intput file\n";
            exit(0);
        }
        assert_file(argv[index]);
        std::ifstream i_file(argv[index]);
        std::ofstream o_file;
        if (argc > ++index)
        {
            o_file.open(argv[index]); 
            assert_file(argv[index]);
        }
        else
        {
            o_file.open(std::string(argv[index - 1]) + ".tof");
        }
        if (argc > ++index)
        {
            std::cerr << argv[0] << ": too many arguments\n";
        }
        graph = input_graph(i_file, checking);
        i_file.close();
        triangles = triangles_of_graph(graph_to_matrix(graph));
        for (size_t i = 0; i < triangles.size(); ++i)
        {
            o_file << graph.vertexes[triangles[i][0]] << " "
                   << graph.vertexes[triangles[i][1]] << " "
                   << graph.vertexes[triangles[i][2]] << std::endl;
        }
        o_file.close();
        return 0;
    }
    std::cout << "Input vertexes and edges of the graph: G(V, E)\n";
    graph = input_graph();
    matrix_t matrix = graph_to_matrix(graph);
    std::cout << "\nmatrix of graph:\n";
    print_matrix(matrix);
    std::cout << std::endl;
    triangles = triangles_of_graph(matrix);
    std::cout << "Triangles of the graph:\n";
    for (size_t i = 0; i < triangles.size(); ++i)
    {
        std::cout << "(" << graph.vertexes[triangles[i][0]] << ", "
                         << graph.vertexes[triangles[i][1]] << ", "
                         << graph.vertexes[triangles[i][2]] << ")\n";
    }
    return 0;
}