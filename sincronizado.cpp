#include <iostream>
#include <unordered_map>
#include <string>
#include <random>
#include <chrono>
#include <vector>
#include <thread>
#include <mutex>

using namespace std;

class Produto {
private:
    string nome;
    int quantidade;
    float valor;
    float desconto;

public:
    Produto(string nome, int quantidade, float valor, float desconto) : nome(nome), quantidade(quantidade), valor(valor), desconto(desconto) {}
    ~Produto() {}

    void adicionarEstoque(int qtd) {
        quantidade += qtd;
    }

    void retirarEstoque(int qtd) {
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
    float getPreco() const { 
        return valor; 
    }
    float getDesconto() const { return desconto; }
};

class Estoque {
private:
    unordered_map<string, Produto> produtos;
    mutex mtx;
    const int max = 50;
    const int min = 0;

public:
    Estoque() {}

    ~Estoque() {}

    void adicionarProduto(const string& nome, int quantidadeInicial, float precoInicial, float descInicial) {
        produtos.emplace(nome, Produto(nome, quantidadeInicial, precoInicial, descInicial));
        //cout << nome << " adicionado ao estoque com quantidade: " << quantidadeInicial << " e preco: " << precoInicial << endl;
    }

    void removerProduto(const string& nome) {
        auto it = produtos.find(nome);
        if (it != produtos.end()) {
            produtos.erase(it);
            //cout << "Produto " << nome << " removido do estoque." << endl;
        }
    }

    // As funções de entrada e saída criam o problema do produtor / consumidor
    void entrada(const string& nome, int qtd) {;
        auto it = produtos.find(nome);
        if (it != produtos.end()) { // Descomente o while para colocar uma verificação de limite
           // while (it->second.getQuantidade() + qtd > max){}
            lock_guard<mutex> lock(mtx);
            it->second.adicionarEstoque(qtd);
           // cout << "Adicionou " << qtd << " unidades de " << nome << " ao estoque." << endl;
        }
    }

    void saida(const string& nome, int qtd) {
        auto it = produtos.find(nome);
        if (it != produtos.end()) {
           // while (it->second.getQuantidade() - qtd < min){}
            lock_guard<mutex> lock(mtx);
            it->second.retirarEstoque(qtd);
            //cout << "Removeu " << qtd << " unidades de " << nome << " do estoque." << endl;
        }
    }

    void novoPreco(const string& nome, float preco) {
        auto it = produtos.find(nome);
        if (it != produtos.end()) {
            lock_guard<mutex> lock(mtx);
            it->second.atualizarPreco(preco);
           // cout << "Preco do produto " << nome << " atualizado para " << preco << endl;
        }
    }

    void novoDesc(const string& nome, float desc) {
        auto it = produtos.find(nome);
        if (it != produtos.end()) {
            lock_guard<mutex> lock(mtx);
            it->second.atualizarDesconto(desc);
            //cout << "Aturalizou desconto do produto " << nome << " para " << desc << endl;
        }
    }

    void exibirEstoque() const {
        cout << "\n--- Estoque Atual ---\n";
        for (const auto& pair : produtos) {
            cout << "Produto: " << pair.second.getNome() << " | Quantidade: " << pair.second.getQuantidade();
            cout << " | Preco: " << pair.second.getPreco() << " | Desconto: " << pair.second.getDesconto() << endl;
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
    for (int i = 0; i < vezes; i++) {
        estoque.entrada("ProdutoA", quantidade);
        estoque.entrada("ProdutoB", quantidade);
        estoque.novoPreco("ProdutoA", gerarValorAleatorio(6.0, 8.0));
        estoque.novoPreco("ProdutoB", gerarValorAleatorio(6.0, 8.0));
        estoque.novoDesc("ProdutoA", gerarValorAleatorio(0.2, 0.5));
        estoque.novoDesc("ProdutoB", gerarValorAleatorio(0.2, 0.5));
    }
}

void simularSaida(Estoque& estoque, int vezes, int quantidade) {
    for (int i = 0; i < vezes; i++) {
        estoque.saida("ProdutoA", quantidade);
        estoque.saida("ProdutoB", quantidade);
        estoque.novoPreco("ProdutoA", gerarValorAleatorio(8.0, 10.0));
        estoque.novoPreco("ProdutoB", gerarValorAleatorio(8.0, 10.0));
        estoque.novoDesc("ProdutoA", gerarValorAleatorio(0.0, 0.2));
        estoque.novoDesc("ProdutoB", gerarValorAleatorio(0.0, 0.2));
    }
}

int main() {
    Estoque estoque;
    int i;
    const int vezes = 50, qtdEntrada = 1, qtdSaida = 1;

    estoque.adicionarProduto("ProdutoA", 0, 5.99, 0.0);
    estoque.adicionarProduto("ProdutoB", 0, 6.66, 0.0);

    auto inicio = chrono::high_resolution_clock::now();
    
    // número de threads = número de trabalhadores
    const int nt = 2;
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