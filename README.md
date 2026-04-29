# UPES BI Portal — Solution Business Intelligence

Portail BI complet pour analyser les données de la faculté UPES (inscriptions, notes, enseignants, finances, recherche).

## Architecture

```
Sources CSV  ──►  ETL Python  ──►  PostgreSQL DWH  ──►  FastAPI  ──►  Vue.js
                  (pandas)          (schéma étoile)    (REST API)   (Dashboard)
                  Airflow DAG                         Superset
```

## Stack technique

| Couche | Outil |
|--------|-------|
| ETL / Génération | Python + pandas |
| Data Warehouse | PostgreSQL 15 (schéma en étoile) |
| Orchestration | Apache Airflow 2.8 |
| Visualisation | Apache Superset 3.1 |
| Backend API | FastAPI + SQLAlchemy |
| Frontend | Vue.js 3 + Chart.js |
| Déploiement | Docker Compose |

## Structure du projet

```
├── data/                    # CSV sources (données fictives générées)
│   ├── etudiants.csv        # 500 étudiants
│   ├── enseignants.csv      # 80 enseignants
│   ├── notes.csv            # 3 000 notes
│   ├── finances.csv         # 200 transactions
│   ├── salles.csv           # 60 salles
│   └── recherche.csv        # 60 projets de recherche
├── etl/
│   ├── generate_data.py     # Génération de données fictives réalistes
│   ├── transform.py         # Nettoyage & construction du schéma en étoile
│   └── load_to_dwh.py       # Chargement dans PostgreSQL
├── sql/
│   ├── create_dimensions.sql  # Tables de dimensions
│   ├── create_facts.sql       # Tables de faits
│   └── create_views.sql       # Vues matérialisées (KPIs)
├── airflow/
│   └── university_pipeline.py # DAG Airflow (exécution quotidienne)
├── api/                     # FastAPI backend
│   ├── main.py
│   ├── models.py
│   ├── deps.py
│   ├── requirements.txt
│   ├── Dockerfile
│   └── routers/
│       ├── auth.py          # JWT login
│       ├── etudiants.py
│       ├── enseignants.py
│       ├── finances.py
│       ├── recherche.py
│       └── kpis.py          # KPIs depuis les vues matérialisées
├── frontend/                # Vue.js 3 SPA
│   ├── src/
│   │   ├── pages/           # Dashboard, Étudiants, Enseignants, Finances, Recherche
│   │   ├── store/auth.js    # Pinia store (JWT)
│   │   ├── router/          # Vue Router
│   │   └── api/             # Axios client
│   ├── Dockerfile
│   └── nginx.conf
└── docker-compose.yml       # PostgreSQL + Airflow + Superset + API + Frontend
```

## Démarrage rapide

### Prérequis
- Docker & Docker Compose
- Python 3.10+ (pour exécuter l'ETL localement)

### 1. Générer les données fictives

```bash
pip install pandas sqlalchemy psycopg2-binary
python etl/generate_data.py
```

### 2. Lancer l'infrastructure complète

```bash
docker compose up -d
```

Services disponibles :
| Service | URL | Identifiants |
|---------|-----|-------------|
| Frontend Vue.js | http://localhost:3000 | admin / admin123 |
| API FastAPI (docs) | http://localhost:8000/docs | — |
| Airflow | http://localhost:8080 | admin / admin |
| Superset | http://localhost:8088 | admin / admin |

### 3. Charger les données dans PostgreSQL

```bash
# Après que PostgreSQL soit démarré
python etl/load_to_dwh.py
```

### 4. Développement local (sans Docker)

```bash
# Backend
cd api
pip install -r requirements.txt
uvicorn main:app --reload

# Frontend
cd frontend
npm install
npm run dev
```

## KPIs implémentés

Les KPIs sont calculés dans des **vues matérialisées PostgreSQL** et exposés via l'API :

| KPI | Endpoint |
|-----|---------|
| Taux de réussite par filière/semestre | `GET /api/kpis/taux-reussite` |
| Taux d'abandon par année | `GET /api/kpis/taux-abandon` |
| Inscrits par filière | `GET /api/kpis/inscrits-par-filiere` |
| Budget vs Dépenses | `GET /api/kpis/budget-depenses` |
| Ratio étudiants/enseignants | `GET /api/kpis/ratio-etudiant-enseignant` |
| Publications par département | `GET /api/kpis/publications` |
| Résumé consolidé | `GET /api/kpis/resume` |

## Authentification

L'API utilise **JWT Bearer tokens**. Comptes disponibles :

| Utilisateur | Mot de passe | Rôle |
|-------------|-------------|------|
| admin | admin123 | admin |
| enseignant | enseignant123 | enseignant |
| etudiant | etudiant123 | etudiant |

## Modèle Data Warehouse (schéma en étoile)

```
          dim_temps
              │
dim_filiere ──┤
              ├── fact_notes ──── dim_module
dim_etudiant ─┤
              │
              └── fact_inscriptions

dim_enseignant ── fact_recherche

                  fact_finances ── dim_temps
```
