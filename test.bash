#!/bin/bash

# Variables par défaut
HOST="127.0.1.1"
PORT="8080"
URL_PATH="/"
REQUESTS=100
CONCURRENCY=10
TIMEOUT=5

# Aide
function show_help {
    echo "Script de benchmark simple pour serveur HTTP"
    echo "Usage: $0 [options]"
    echo "Options:"
    echo "  -h, --host HOST       Hôte à tester (défaut: localhost)"
    echo "  -p, --port PORT       Port à utiliser (défaut: 8080)"
    echo "  -u, --path PATH       Chemin d'URL (défaut: /)"
    echo "  -n, --requests N      Nombre de requêtes (défaut: 100)"
    echo "  -c, --concurrency N   Niveau de concurrence (défaut: 10)"
    echo "  -t, --timeout SEC     Timeout en secondes (défaut: 5)"
    echo "  --help                Afficher cette aide"
    exit 1
}

# Traitement des paramètres
while [[ $# -gt 0 ]]; do
    case $1 in
        -h|--host) HOST="$2"; shift 2 ;;
        -p|--port) PORT="$2"; shift 2 ;;
        -u|--path) URL_PATH="$2"; shift 2 ;;
        -n|--requests) REQUESTS="$2"; shift 2 ;;
        -c|--concurrency) CONCURRENCY="$2"; shift 2 ;;
        -t|--timeout) TIMEOUT="$2"; shift 2 ;;
        --help) show_help ;;
        *) echo "Option inconnue: $1"; show_help ;;
    esac
done

echo "Benchmark sur http://$HOST:$PORT$URL_PATH"
echo "Requêtes: $REQUESTS, Concurrence: $CONCURRENCY"
echo "Démarrage du benchmark..."

# Initialisation des compteurs
start_time=$(date +%s)
success=0
failed=0
slowest=0
fastest=999999
total_time=0

# Fonction pour faire une requête HTTP basique avec /dev/tcp
function do_request {
    local start=$(date +%s.%N)
    local status=0
    
    # Création d'une requête HTTP basique
    {
        echo -e "GET $URL_PATH HTTP/1.1"
        echo -e "Host: $HOST"
        echo -e "Connection: close"
        echo -e "\r\n"
    } > /dev/tcp/$HOST/$PORT 2>/dev/null
    
    if [ $? -eq 0 ]; then
        status=200
    else
        status=500
    fi
    
    local end=$(date +%s.%N)
    local time=$(echo "$end - $start" | bc)
    
    # Mise à jour des statistiques
    echo "$status $time"
}

# Fonction pour exécuter les tests
function run_tests {
    local pid_list=()
    local results=()
    
    # Lancer les requêtes en parallèle
    for ((i=1; i<=$REQUESTS; i++)); do
        # Affichage de la progression
        if (( i % 10 == 0 )); then
            echo -ne "Progression: $i/$REQUESTS\r"
        fi
        
        # Lancer la requête en arrière-plan
        do_request > /tmp/benchmark_$$_$i &
        pid_list+=($!)
        
        # Limiter la concurrence
        if (( ${#pid_list[@]} >= CONCURRENCY )); then
            # Attendre que la première requête se termine
            wait ${pid_list[0]}
            pid_list=("${pid_list[@]:1}")
        fi
    done
    
    # Attendre que toutes les requêtes se terminent
    for pid in "${pid_list[@]}"; do
        wait $pid
    done
    
    # Collecter les résultats
    for ((i=1; i<=$REQUESTS; i++)); do
        if [ -f /tmp/benchmark_$$_$i ]; then
            local result=$(cat /tmp/benchmark_$$_$i)
            local status=$(echo $result | cut -d' ' -f1)
            local time=$(echo $result | cut -d' ' -f2)
            
            if [ "$status" == "200" ]; then
                ((success++))
            else
                ((failed++))
            fi
            
            # Mettre à jour les statistiques de temps
            total_time=$(echo "$total_time + $time" | bc)
            
            if (( $(echo "$time > $slowest" | bc -l) )); then
                slowest=$time
            fi
            
            if (( $(echo "$time < $fastest" | bc -l) )); then
                fastest=$time
            fi
            
            # Supprimer le fichier temporaire
            rm /tmp/benchmark_$$_$i
        else
            ((failed++))
        fi
    done
}

# Exécuter les tests
run_tests

# Calcul des statistiques
end_time=$(date +%s)
total_duration=$((end_time - start_time))
avg_time=$(echo "scale=6; $total_time / $success" | bc 2>/dev/null)
if [ -z "$avg_time" ]; then
    avg_time="N/A"
fi
requests_per_sec=$(echo "scale=2; $REQUESTS / $total_duration" | bc)

# Affichage des résultats
echo ""
echo "=== Résultats du benchmark ==="
echo "Requêtes totales:      $REQUESTS"
echo "Requêtes réussies:     $success"
echo "Requêtes échouées:     $failed"
echo "Temps total:           ${total_duration}s"
echo "Requêtes par seconde:  $requests_per_sec"
if [ "$avg_time" != "N/A" ]; then
    echo "Temps moyen:           ${avg_time}s"
    echo "Temps le plus rapide:  ${fastest}s"
    echo "Temps le plus lent:    ${slowest}s"
fi
echo "============================="