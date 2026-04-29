"""
routers/kpis.py
Endpoints pour les KPIs principaux (vues matérialisées).
"""

from typing import Optional
from fastapi import APIRouter, Query
from sqlalchemy import text

from deps import DbDep, CurrentUser

router = APIRouter()


@router.get("/taux-reussite", summary="Taux de réussite par filière et semestre")
def kpi_taux_reussite(
    db: DbDep,
    _: CurrentUser,
    annee: Optional[int] = Query(None),
    filiere: Optional[str] = Query(None),
):
    sql = "SELECT * FROM kpi_taux_reussite WHERE 1=1"
    params = {}
    if annee:
        sql += " AND annee = :annee"
        params["annee"] = annee
    if filiere:
        sql += " AND nom_filiere = :filiere"
        params["filiere"] = filiere
    sql += " ORDER BY annee, semestre, nom_filiere"
    rows = db.execute(text(sql), params).mappings().all()
    return list(rows)


@router.get("/taux-abandon", summary="Taux d'abandon par année et filière")
def kpi_taux_abandon(
    db: DbDep,
    _: CurrentUser,
    annee: Optional[int] = Query(None),
):
    sql = "SELECT * FROM kpi_taux_abandon"
    params = {}
    if annee:
        sql += " WHERE annee = :annee"
        params["annee"] = annee
    sql += " ORDER BY annee, nom_filiere"
    rows = db.execute(text(sql), params).mappings().all()
    return list(rows)


@router.get("/inscrits-par-filiere", summary="Nombre d'inscrits par filière")
def kpi_inscrits(
    db: DbDep,
    _: CurrentUser,
    annee: Optional[int] = Query(None),
):
    sql = "SELECT * FROM kpi_inscrits_par_filiere"
    params = {}
    if annee:
        sql += " WHERE annee = :annee"
        params["annee"] = annee
    sql += " ORDER BY annee, nom_filiere"
    rows = db.execute(text(sql), params).mappings().all()
    return list(rows)


@router.get("/budget-depenses", summary="Budget vs Dépenses réelles par département")
def kpi_budget(
    db: DbDep,
    _: CurrentUser,
    annee: Optional[int] = Query(None),
    departement: Optional[str] = Query(None),
):
    sql = "SELECT * FROM kpi_budget_depenses WHERE 1=1"
    params = {}
    if annee:
        sql += " AND annee = :annee"
        params["annee"] = annee
    if departement:
        sql += " AND departement = :departement"
        params["departement"] = departement
    sql += " ORDER BY annee, departement"
    rows = db.execute(text(sql), params).mappings().all()
    return list(rows)


@router.get("/ratio-etudiant-enseignant", summary="Ratio étudiants / enseignants")
def kpi_ratio(
    db: DbDep,
    _: CurrentUser,
    annee: Optional[int] = Query(None),
):
    sql = "SELECT * FROM kpi_ratio_etudiant_enseignant"
    params = {}
    if annee:
        sql += " WHERE annee = :annee"
        params["annee"] = annee
    sql += " ORDER BY annee, departement"
    rows = db.execute(text(sql), params).mappings().all()
    return list(rows)


@router.get("/publications", summary="Publications par département")
def kpi_publications(db: DbDep, _: CurrentUser):
    rows = db.execute(
        text("SELECT * FROM kpi_publications_departement ORDER BY total_publications DESC")
    ).mappings().all()
    return list(rows)


@router.get("/resume", summary="Résumé de tous les KPIs")
def kpi_resume(db: DbDep, _: CurrentUser):
    """Retourne un résumé consolidé de tous les KPIs pour le dashboard principal."""
    nb_etudiants = db.execute(text("SELECT COUNT(*) FROM dim_etudiant")).scalar()
    nb_enseignants = db.execute(text("SELECT COUNT(*) FROM dim_enseignant")).scalar()
    nb_projets = db.execute(text("SELECT COUNT(*) FROM fact_recherche")).scalar()
    total_publications = db.execute(
        text("SELECT COALESCE(SUM(publications), 0) FROM fact_recherche")
    ).scalar()
    taux_reussite_moy = db.execute(
        text("SELECT ROUND(AVG(taux_reussite_pct), 2) FROM kpi_taux_reussite")
    ).scalar()
    taux_abandon_moy = db.execute(
        text("SELECT ROUND(AVG(taux_abandon_pct), 2) FROM kpi_taux_abandon")
    ).scalar()
    solde_global = db.execute(
        text("SELECT ROUND(SUM(solde), 2) FROM kpi_budget_depenses")
    ).scalar()

    return {
        "nb_etudiants": nb_etudiants,
        "nb_enseignants": nb_enseignants,
        "nb_projets_recherche": nb_projets,
        "total_publications": total_publications,
        "taux_reussite_moyen_pct": float(taux_reussite_moy or 0),
        "taux_abandon_moyen_pct": float(taux_abandon_moy or 0),
        "solde_financier_global": float(solde_global or 0),
    }
