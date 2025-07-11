#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <random> // Para simular valores de transação aleatórios
#include <chrono> // Para simular tempo de trabalho (sleep)
#include <numeric> // para calcular a média
#include <atomic> // para a contagem de produtores ativos
#include <iomanip>
using namespace std;
const int tamanho_buffer= 5 //capacidade do buffer
const int num_produtores = 3 // nùmero de threads de caixa

vector<int> buffer_vendas//buffer que aramazena valores de vendas
mutex mutex_buffer;//protege o acesso ao buffer

condition_variable cv_produtor;// pordutores vão esperar aqui se o buffer tiver cheio
condition_variable cv_consumidor; // consumidores vão esperar aqui se o buffer tiver vazio

std::atomic<int> num_produtores_ativos(0);//contar produtores ativos

random_device dispositivo_aleatorio;
mt19937 gerador_numeros_aleatorios(dispositivo_aleatorio());
uniform_int_distribution<> distrib_valor_venda(1, 1000);   // Valores de venda entre 1 e 1000 reais
uniform_int_distribution<> distrib_num_vendas(20, 30);     // Qtd. de vendas por produtor (entre 20 e 30)
uniform_int_distribution<> distrib_delay_segundos(1, 5); //  // Delay aleatório (1 a 5 segundos)

void* produtor(void* args) {
    // Desempacota o parâmetro: um array de int com {id_caixa, total_vendas_a_produzir}
    int* params = static_cast<int*>(args);
    int id_caixa = params[0];
    int total_vendas_a_produzir = params[1];
    delete[] params;

    num_produtores_ativos++; 
    
    // Obtém o ID da thread
    hash<thread::id> hasher;
    size_t tid_hash = hasher(this_thread::get_id());

    for (int i = 0; i < total_vendas_a_produzir; ++i) {
        int valor_venda = distrib_valor_venda(gerador); 
        unique_lock<mutex> lock(mtx_buffer);

        // Espera se o buffer estiver cheio
        cv_produtor.wait(lock, [] { 
            return buffer_vendas.size() < TAMANHO_BUFFER; 
        });

        buffer_vendas.push_back(valor_venda);
        
       cout << "(P) TID: " << tid_hash << " | VALOR: R$ " << valor_venda 
                  << " | ITERACAO: " << (i + 1) << endl;

        // Notifica o consumidor se o buffer ficou cheio ou se é a última venda do produtor
        // e o consumidor pode precisar processar o que sobrou.
        if (buffer_vendas.size() == TAMANHO_BUFFER || (i == total_vendas_a_produzir - 1)) {
            cv_consumidor.notify_one(); 
        }

        lock.unlock(); 

        // Espera um tempo aleatório antes de produzir a próxima venda
        this_thread::sleep_for(chrono::seconds(distrib_delay_segundos(gerador)));
    }

    num_produtores_ativos--;
    
    
    cout << "(P) TID: " << tid_hash << " finalizou" << endl;
    
    cv_consumidor.notify_one(); 

    return NULL;
}
void * consumer ( void * args ) {
    while ( /* tem produtoras ainda ? */ ) {
        // esperar 5 dados
        // acessar buffer compartilhado ( consumir )
        // imprimir TID / dados
    }
    // imprimir que finalizou
}

int main ( void ) {
    // criar threads produtoras
    // criar thread consumidora
    // esperar threads terminarem
}
