"""
routers/enseignants.py
Endpoints pour les données enseignants.
"""

from typing import Optional
from fastapi import APIRouter, Query, HTTPException
from pydantic import BaseModel
from sqlalchemy import func, select

from deps import DbDep, CurrentUser
from models import DimEnseignant

router = APIRouter()


class EnseignantOut(BaseModel):
    enseignant_id: int
    nom: str
    grade: Optional[str] = None
    departement: Optional[str] = None
    email: Optional[str] = None

    model_config = {"from_attributes": True}


@router.get("/", response_model=list[EnseignantOut])
def list_enseignants(
    db: DbDep,
    _: CurrentUser,
    departement: Optional[str] = Query(None),
    grade: Optional[str] = Query(None),
    limit: int = Query(50, ge=1, le=200),
    offset: int = Query(0, ge=0),
):
    stmt = select(DimEnseignant)
    if departement:
        stmt = stmt.where(DimEnseignant.departement == departement)
    if grade:
        stmt = stmt.where(DimEnseignant.grade == grade)
    stmt = stmt.limit(limit).offset(offset)
    return db.execute(stmt).scalars().all()


@router.get("/stats", summary="Statistiques globales des enseignants")
def stats_enseignants(db: DbDep, _: CurrentUser):
    total = db.execute(select(func.count()).select_from(DimEnseignant)).scalar()
    par_grade = db.execute(
        select(DimEnseignant.grade, func.count().label("nb"))
        .group_by(DimEnseignant.grade)
        .order_by(func.count().desc())
    ).all()
    par_dept = db.execute(
        select(DimEnseignant.departement, func.count().label("nb"))
        .group_by(DimEnseignant.departement)
        .order_by(func.count().desc())
    ).all()
    return {
        "total": total,
        "par_grade": [{"grade": g, "nb": n} for g, n in par_grade],
        "par_departement": [{"departement": d, "nb": n} for d, n in par_dept],
    }


@router.get("/{enseignant_id}", response_model=EnseignantOut)
def get_enseignant(enseignant_id: int, db: DbDep, _: CurrentUser):
    obj = db.get(DimEnseignant, enseignant_id)
    if not obj:
        raise HTTPException(status_code=404, detail="Enseignant introuvable")
    return obj
