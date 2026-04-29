"""
load_to_dwh.py
Charge les données transformées dans le Data Warehouse PostgreSQL.
"""

import os
import sys
import logging
from sqlalchemy import create_engine, text

# Importer le pipeline de transformation
sys.path.insert(0, os.path.dirname(__file__))
from transform import run_transformations

logging.basicConfig(
    level=logging.INFO,
    format="%(asctime)s %(levelname)s %(message)s",
)
logger = logging.getLogger(__name__)

DATABASE_URL = os.getenv(
    "DATABASE_URL",
    "postgresql+psycopg2://upes:upes_password@localhost:5432/upes_dwh",
)


def get_engine():
    return create_engine(DATABASE_URL, echo=False)


def load_schema(engine):
    """Exécute les fichiers SQL de création du schéma."""
    sql_dir = os.path.join(os.path.dirname(__file__), "..", "sql")
    for fname in ["create_dimensions.sql", "create_facts.sql", "create_views.sql"]:
        fpath = os.path.join(sql_dir, fname)
        if not os.path.exists(fpath):
            logger.warning("Fichier SQL introuvable : %s", fpath)
            continue
        with open(fpath, "r", encoding="utf-8") as f:
            sql = f.read()
        with engine.begin() as conn:
            conn.execute(text(sql))
        logger.info("Schéma appliqué : %s", fname)


def load_table(engine, df, table_name, if_exists="replace"):
    """Charge un DataFrame dans une table PostgreSQL."""
    rows = len(df)
    df.to_sql(table_name, engine, if_exists=if_exists, index=False, method="multi", chunksize=500)
    logger.info("Table chargée : %-25s (%d lignes)", table_name, rows)


def refresh_materialized_views(engine):
    """Rafraîchit les vues matérialisées des KPIs."""
    views = [
        "kpi_taux_reussite",
        "kpi_taux_abandon",
        "kpi_inscrits_par_filiere",
        "kpi_budget_depenses",
        "kpi_ratio_etudiant_enseignant",
        "kpi_publications_departement",
    ]
    with engine.begin() as conn:
        for view in views:
            conn.execute(text(f"REFRESH MATERIALIZED VIEW {view}"))
            logger.info("Vue rafraîchie : %s", view)


def run_etl():
    logger.info("=== Démarrage du pipeline ETL UPES ===")
    engine = get_engine()

    logger.info("--- Étape 1 : Application du schéma SQL ---")
    load_schema(engine)

    logger.info("--- Étape 2 : Transformation des données ---")
    tables = run_transformations()

    logger.info("--- Étape 3 : Chargement dans le Data Warehouse ---")
    # Ordre de chargement respectant les FK
    order = [
        "dim_temps",
        "dim_filiere",
        "dim_etudiant",
        "dim_enseignant",
        "dim_module",
        "dim_salle",
        "fact_inscriptions",
        "fact_notes",
        "fact_finances",
        "fact_recherche",
    ]
    for table_name in order:
        df = tables.get(table_name)
        if df is not None:
            load_table(engine, df, table_name, if_exists="replace")

    logger.info("--- Étape 4 : Rafraîchissement des vues KPI ---")
    try:
        refresh_materialized_views(engine)
    except Exception as exc:
        logger.warning("Rafraîchissement des vues échoué (peut survenir au 1er run) : %s", exc)

    logger.info("=== Pipeline ETL terminé avec succès ===")


if __name__ == "__main__":
    run_etl()
