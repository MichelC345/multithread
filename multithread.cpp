#include <iostream>
#include <list>
#include <string>
#include <random>
#include <chrono>
#include <vector>
#include <thread>
#include <fstream>

using namespace std;

class Produto {
private:
    string nome;
    int quantidade;
    float valor;
    float desconto;
    const int max = 50;
    const int min = 0;

public:
    Produto(string nome, int quantidade, float valor, float desconto) : nome(nome), quantidade(quantidade), valor(valor), desconto(desconto) {}
    ~Produto() {}

    void adicionarEstoque(int qtd) { // Descomente o while para colocar uma verificação de limite
        //while(quantidade + qtd > max){}
        quantidade += qtd;
    }

    void retirarEstoque(int qtd) { // Descomente o while para colocar uma verificação de limite
       // while(quantidade - qtd < min){}
        quantidade -= qtd;
    }

    void atualizarPreco(float preco) {
        valor = preco;
    }

    void atualizarDesconto(float desc) {
        desconto = desc;
    }

    int getQuantidade() const { return quantidade; }
    string getNome() const { return nome; }
    float getPreco() const { return valor; }
    float getDesconto() const { return desconto; }
};

class Estoque {
private:
    list<Produto> produtos; 

public:
    Estoque() {}

    ~Estoque() {}

    void adicionarProduto(const string& nome, int quantidadeInicial, float precoInicial, float descInicial) {
        produtos.emplace_back(nome, quantidadeInicial, precoInicial, descInicial);
    }

    void removerProduto(const string& nome) {
        produtos.remove_if([&](const Produto& p) { return p.getNome() == nome; });
    }

    list<Produto>& getProdutos() {
        return produtos;
    }

    // Busca o produto pelo nome
    Produto* buscarProduto(const string& nome) {
        for (auto& produto : produtos) {
            if (produto.getNome() == nome) {
                return &produto;
            }
        }
        return nullptr;
    }

    // Funções de entrada e saída
    void entrada(const string& nome, int qtd) {
        Produto* produto = buscarProduto(nome);
        if (produto) {
            produto->adicionarEstoque(qtd);
        }
    }

    void saida(const string& nome, int qtd) {
        Produto* produto = buscarProduto(nome);
        if (produto) {
            produto->retirarEstoque(qtd);
        }
    }

    void novoPreco(const string& nome, float preco) {
        Produto* produto = buscarProduto(nome);
        if (produto) {
            produto->atualizarPreco(preco);
        }
    }

    void novoDesc(const string& nome, float desc) {
        Produto* produto = buscarProduto(nome);
        if (produto) {
            produto->atualizarDesconto(desc);
        }
    }

    void exibirEstoque() const {
        cout << "\n--- Estoque Atual ---\n";
        for (const auto& produto : produtos) {
            cout << "Produto: " << produto.getNome() << " | Quantidade: " << produto.getQuantidade();
            cout << " | Preco: " << produto.getPreco() << " | Desconto: " << produto.getDesconto() << endl;
        }
    }
};

float gerarValorAleatorio(float min, float max) {
    static random_device rd;                           
    static mt19937 gen(rd());                          
    uniform_real_distribution<> dist(min, max);   
    return dist(gen);
}

void simularEntrada(Estoque& estoque, int vezes, int quantidade) {
    auto& produtos = estoque.getProdutos();
    for (int i = 0; i < vezes; i++) {
        // Percorre a lista produtos modificando a quantidade, preço e desconto
        for (auto& produto : produtos) {
            estoque.entrada(produto.getNome(), quantidade);
            estoque.novoPreco(produto.getNome(), gerarValorAleatorio(5.0, 11.0));
            estoque.novoDesc(produto.getNome(), gerarValorAleatorio(0.1, 0.5));
        }
    }
}

void simularSaida(Estoque& estoque, int vezes, int quantidade) {
    auto& produtos = estoque.getProdutos();
    for (int i = 0; i < vezes; i++) {
        // Percorre a lista produtos modificando a quantidade, preço e desconto
        for (auto& produto : produtos) {
            estoque.saida(produto.getNome(), quantidade);
            estoque.novoPreco(produto.getNome(), gerarValorAleatorio(10.0, 20.0));
            estoque.novoDesc(produto.getNome(), gerarValorAleatorio(0.0, 0.2));
        }
    }
}

void lerProdutosArquivo(const string& nomeArquivo, Estoque& estoque) {
    ifstream arquivo(nomeArquivo);
    if (!arquivo) {
        cerr << "Erro ao abrir o arquivo " << nomeArquivo << endl;
        return;
    }

    string nome;
    int quantidade;
    float preco, desconto;
    while (arquivo >> nome >> quantidade >> preco >> desconto) {
        estoque.adicionarProduto(nome, quantidade, preco, desconto);
    }
    arquivo.close();
}

int main() {
    Estoque estoque;
    int i;
    const int vezes = 500, qtdEntrada = 1, qtdSaida = 1;

    lerProdutosArquivo("produtos.txt", estoque);

    auto inicio = chrono::high_resolution_clock::now();
    
    const int nt = 10;
    vector<thread> threads;
    for(i = 0; i < nt; i++){
        threads.emplace_back(simularEntrada, ref(estoque), vezes, qtdEntrada);
        threads.emplace_back(simularSaida, ref(estoque), vezes, qtdSaida);
    }
    
    for (auto& t : threads) {
        t.join();
    }

    auto fim = chrono::high_resolution_clock::now();
    chrono::duration<double> duracao = fim - inicio;

    estoque.exibirEstoque();
    cout << "\nTempo de execução: " << duracao.count() << " segundos" << endl;

    return 0;
}
