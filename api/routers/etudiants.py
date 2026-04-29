"""
routers/etudiants.py
Endpoints pour les données étudiants.
"""

from typing import Optional
from fastapi import APIRouter, Query
from pydantic import BaseModel
from sqlalchemy import func, select

from deps import DbDep, CurrentUser
from models import DimEtudiant, DimFiliere

router = APIRouter()


class EtudiantOut(BaseModel):
    etudiant_id: int
    nom: str
    filiere: Optional[str] = None
    annee_inscription: Optional[int] = None
    statut: Optional[str] = None
    email: Optional[str] = None
    sexe: Optional[str] = None

    model_config = {"from_attributes": True}


@router.get("/", response_model=list[EtudiantOut])
def list_etudiants(
    db: DbDep,
    _: CurrentUser,
    filiere: Optional[str] = Query(None, description="Filtrer par filière"),
    statut: Optional[str] = Query(None, description="Filtrer par statut"),
    annee: Optional[int] = Query(None, description="Filtrer par année d'inscription"),
    limit: int = Query(50, ge=1, le=200),
    offset: int = Query(0, ge=0),
):
    stmt = (
        select(DimEtudiant, DimFiliere.nom_filiere)
        .outerjoin(DimFiliere, DimEtudiant.filiere_id == DimFiliere.filiere_id)
    )
    if filiere:
        stmt = stmt.where(DimFiliere.nom_filiere == filiere)
    if statut:
        stmt = stmt.where(DimEtudiant.statut == statut)
    if annee:
        stmt = stmt.where(DimEtudiant.annee_inscription == annee)
    stmt = stmt.limit(limit).offset(offset)

    rows = db.execute(stmt).all()
    return [
        EtudiantOut(
            etudiant_id=e.etudiant_id,
            nom=e.nom,
            filiere=nom_filiere,
            annee_inscription=e.annee_inscription,
            statut=e.statut,
            email=e.email,
            sexe=e.sexe,
        )
        for e, nom_filiere in rows
    ]


@router.get("/stats", summary="Statistiques globales des étudiants")
def stats_etudiants(db: DbDep, _: CurrentUser):
    total = db.execute(select(func.count()).select_from(DimEtudiant)).scalar()
    par_statut = db.execute(
        select(DimEtudiant.statut, func.count().label("nb"))
        .group_by(DimEtudiant.statut)
    ).all()
    par_filiere = db.execute(
        select(DimFiliere.nom_filiere, func.count().label("nb"))
        .join(DimEtudiant, DimEtudiant.filiere_id == DimFiliere.filiere_id)
        .group_by(DimFiliere.nom_filiere)
        .order_by(func.count().desc())
    ).all()
    return {
        "total": total,
        "par_statut": [{"statut": s, "nb": n} for s, n in par_statut],
        "par_filiere": [{"filiere": f, "nb": n} for f, n in par_filiere],
    }


@router.get("/{etudiant_id}", response_model=EtudiantOut)
def get_etudiant(etudiant_id: int, db: DbDep, _: CurrentUser):
    from fastapi import HTTPException
    row = db.execute(
        select(DimEtudiant, DimFiliere.nom_filiere)
        .outerjoin(DimFiliere, DimEtudiant.filiere_id == DimFiliere.filiere_id)
        .where(DimEtudiant.etudiant_id == etudiant_id)
    ).first()
    if not row:
        raise HTTPException(status_code=404, detail="Étudiant introuvable")
    e, nom_filiere = row
    return EtudiantOut(
        etudiant_id=e.etudiant_id,
        nom=e.nom,
        filiere=nom_filiere,
        annee_inscription=e.annee_inscription,
        statut=e.statut,
        email=e.email,
        sexe=e.sexe,
    )
