"""
routers/recherche.py
Endpoints pour la recherche scientifique.
"""

from typing import Optional
from fastapi import APIRouter, Query, HTTPException
from pydantic import BaseModel
from decimal import Decimal
from sqlalchemy import func, select

from deps import DbDep, CurrentUser
from models import FactRecherche, DimEnseignant

router = APIRouter()


class RechercheOut(BaseModel):
    projet_id: int
    departement: Optional[str] = None
    thematique: Optional[str] = None
    budget: Optional[Decimal] = None
    depenses: Optional[Decimal] = None
    statut: Optional[str] = None
    publications: Optional[int] = None

    model_config = {"from_attributes": True}


@router.get("/", response_model=list[RechercheOut])
def list_recherche(
    db: DbDep,
    _: CurrentUser,
    departement: Optional[str] = Query(None),
    statut: Optional[str] = Query(None),
    thematique: Optional[str] = Query(None),
    limit: int = Query(50, ge=1, le=200),
    offset: int = Query(0, ge=0),
):
    stmt = select(FactRecherche)
    if departement:
        stmt = stmt.where(FactRecherche.departement == departement)
    if statut:
        stmt = stmt.where(FactRecherche.statut == statut)
    if thematique:
        stmt = stmt.where(FactRecherche.thematique.ilike(f"%{thematique}%"))
    stmt = stmt.limit(limit).offset(offset)
    return db.execute(stmt).scalars().all()


@router.get("/stats", summary="Statistiques de recherche par département")
def stats_recherche(db: DbDep, _: CurrentUser):
    rows = db.execute(
        select(
            FactRecherche.departement,
            func.count().label("nb_projets"),
            func.sum(FactRecherche.publications).label("total_publications"),
            func.sum(FactRecherche.budget).label("budget_total"),
            func.sum(FactRecherche.depenses).label("depenses_totales"),
        ).group_by(FactRecherche.departement)
        .order_by(func.sum(FactRecherche.publications).desc())
    ).all()
    return [
        {
            "departement": r.departement,
            "nb_projets": r.nb_projets,
            "total_publications": r.total_publications or 0,
            "budget_total": float(r.budget_total or 0),
            "depenses_totales": float(r.depenses_totales or 0),
        }
        for r in rows
    ]


@router.get("/{projet_id}", response_model=RechercheOut)
def get_projet(projet_id: int, db: DbDep, _: CurrentUser):
    obj = db.get(FactRecherche, projet_id)
    if not obj:
        raise HTTPException(status_code=404, detail="Projet introuvable")
    return obj
