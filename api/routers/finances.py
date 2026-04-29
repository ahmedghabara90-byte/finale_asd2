"""
routers/finances.py
Endpoints pour les données financières.
"""

from typing import Optional
from fastapi import APIRouter, Query
from pydantic import BaseModel
from decimal import Decimal
from sqlalchemy import func, select

from deps import DbDep, CurrentUser
from models import FactFinance

router = APIRouter()


class FinanceOut(BaseModel):
    finance_id: int
    annee: int
    poste_budgetaire: Optional[str] = None
    type: Optional[str] = None
    montant: Decimal
    departement: Optional[str] = None
    description: Optional[str] = None

    model_config = {"from_attributes": True}


@router.get("/", response_model=list[FinanceOut])
def list_finances(
    db: DbDep,
    _: CurrentUser,
    annee: Optional[int] = Query(None),
    type_trans: Optional[str] = Query(None, alias="type"),
    departement: Optional[str] = Query(None),
    limit: int = Query(50, ge=1, le=200),
    offset: int = Query(0, ge=0),
):
    stmt = select(FactFinance)
    if annee:
        stmt = stmt.where(FactFinance.annee == annee)
    if type_trans:
        stmt = stmt.where(FactFinance.type == type_trans)
    if departement:
        stmt = stmt.where(FactFinance.departement == departement)
    stmt = stmt.limit(limit).offset(offset)
    return db.execute(stmt).scalars().all()


@router.get("/resume", summary="Résumé financier par année")
def resume_finances(db: DbDep, _: CurrentUser, annee: Optional[int] = Query(None)):
    stmt = select(
        FactFinance.annee,
        FactFinance.type,
        func.sum(FactFinance.montant).label("total"),
        func.count().label("nb_transactions"),
    ).group_by(FactFinance.annee, FactFinance.type)

    if annee:
        stmt = stmt.where(FactFinance.annee == annee)

    stmt = stmt.order_by(FactFinance.annee, FactFinance.type)
    rows = db.execute(stmt).all()
    return [
        {
            "annee": r.annee,
            "type": r.type,
            "total": float(r.total),
            "nb_transactions": r.nb_transactions,
        }
        for r in rows
    ]


@router.get("/par-poste", summary="Dépenses par poste budgétaire")
def finances_par_poste(
    db: DbDep,
    _: CurrentUser,
    annee: Optional[int] = Query(None),
):
    stmt = select(
        FactFinance.poste_budgetaire,
        FactFinance.type,
        func.sum(FactFinance.montant).label("total"),
    ).group_by(FactFinance.poste_budgetaire, FactFinance.type)
    if annee:
        stmt = stmt.where(FactFinance.annee == annee)
    rows = db.execute(stmt).all()
    return [
        {"poste": r.poste_budgetaire, "type": r.type, "total": float(r.total)}
        for r in rows
    ]
