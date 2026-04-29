"""
university_pipeline.py
DAG Airflow pour automatiser le pipeline ETL de l'université UPES.
Exécution : quotidienne (00:30)
"""

from datetime import datetime, timedelta

from airflow import DAG
from airflow.operators.python import PythonOperator
from airflow.operators.bash import BashOperator

import sys
import os

ETL_PATH = os.path.join(os.path.dirname(__file__), "..", "etl")
sys.path.insert(0, ETL_PATH)

default_args = {
    "owner": "upes_bi",
    "depends_on_past": False,
    "email_on_failure": False,
    "email_on_retry": False,
    "retries": 2,
    "retry_delay": timedelta(minutes=5),
}

with DAG(
    dag_id="upes_etl_pipeline",
    default_args=default_args,
    description="Pipeline ETL complet pour le Data Warehouse UPES",
    schedule_interval="30 0 * * *",
    start_date=datetime(2024, 1, 1),
    catchup=False,
    tags=["upes", "etl", "bi"],
) as dag:

    # ── Tâche 1 : Générer les données (si nécessaire) ───────────────────────
    generate_data = BashOperator(
        task_id="generate_data",
        bash_command=(
            "python /opt/airflow/etl/generate_data.py "
            "&& echo 'Données générées avec succès'"
        ),
    )

    # ── Tâche 2 : Transformer et charger dans le DWH ────────────────────────
    def _run_etl():
        from load_to_dwh import run_etl
        run_etl()

    load_dwh = PythonOperator(
        task_id="load_to_dwh",
        python_callable=_run_etl,
    )

    # ── Tâche 3 : Rafraîchir les vues matérialisées ─────────────────────────
    def _refresh_views():
        import os
        from sqlalchemy import create_engine, text
        db_url = os.getenv(
            "DATABASE_URL",
            "postgresql+psycopg2://upes:upes_password@postgres:5432/upes_dwh",
        )
        engine = create_engine(db_url)
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
                print(f"Vue rafraîchie : {view}")

    refresh_views = PythonOperator(
        task_id="refresh_kpi_views",
        python_callable=_refresh_views,
    )

    # ── Tâche 4 : Vérification de qualité des données ───────────────────────
    def _data_quality_check():
        import os
        from sqlalchemy import create_engine, text
        db_url = os.getenv(
            "DATABASE_URL",
            "postgresql+psycopg2://upes:upes_password@postgres:5432/upes_dwh",
        )
        engine = create_engine(db_url)
        checks = {
            "dim_etudiant": "SELECT COUNT(*) FROM dim_etudiant",
            "fact_notes": "SELECT COUNT(*) FROM fact_notes",
            "fact_inscriptions": "SELECT COUNT(*) FROM fact_inscriptions",
            "notes_invalides": "SELECT COUNT(*) FROM fact_notes WHERE note < 0 OR note > 20",
        }
        with engine.connect() as conn:
            for name, query in checks.items():
                count = conn.execute(text(query)).scalar()
                print(f"  {name}: {count}")
                if name == "notes_invalides" and count > 0:
                    raise ValueError(f"Notes invalides détectées : {count}")

    data_quality = PythonOperator(
        task_id="data_quality_check",
        python_callable=_data_quality_check,
    )

    # ── Orchestration ────────────────────────────────────────────────────────
    generate_data >> load_dwh >> refresh_views >> data_quality
