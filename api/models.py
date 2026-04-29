"""
models.py
Modèles SQLAlchemy pour le Data Warehouse UPES.
"""

from datetime import date
from typing import Optional

from sqlalchemy import (
    Boolean, Column, Date, Integer, Numeric,
    SmallInteger, String, Text, ForeignKey, CheckConstraint, Computed,
)
from sqlalchemy.orm import DeclarativeBase, relationship


class Base(DeclarativeBase):
    pass


# ── Dimensions ────────────────────────────────────────────────────────────────

class DimTemps(Base):
    __tablename__ = "dim_temps"
    temps_id     = Column(Integer, primary_key=True, autoincrement=True)
    date_value   = Column(Date, nullable=False, unique=True)
    jour         = Column(SmallInteger, nullable=False)
    mois         = Column(SmallInteger, nullable=False)
    trimestre    = Column(SmallInteger, nullable=False)
    annee        = Column(SmallInteger, nullable=False)
    semestre     = Column(SmallInteger, nullable=False)
    nom_mois     = Column(String(20), nullable=False)
    jour_semaine = Column(String(15), nullable=False)


class DimFiliere(Base):
    __tablename__ = "dim_filiere"
    filiere_id   = Column(Integer, primary_key=True, autoincrement=True)
    nom_filiere  = Column(String(100), nullable=False, unique=True)
    departement  = Column(String(100))
    niveau       = Column(String(50), default="Licence")
    duree_annees = Column(SmallInteger, default=3)

    etudiants    = relationship("DimEtudiant", back_populates="filiere")
    modules      = relationship("DimModule", back_populates="filiere")


class DimEtudiant(Base):
    __tablename__ = "dim_etudiant"
    etudiant_id       = Column(Integer, primary_key=True)
    nom               = Column(String(150), nullable=False)
    filiere_id        = Column(Integer, ForeignKey("dim_filiere.filiere_id"))
    annee_inscription = Column(SmallInteger)
    statut            = Column(String(30))
    date_naissance    = Column(Date)
    email             = Column(String(150))
    sexe              = Column(String(1))

    filiere = relationship("DimFiliere", back_populates="etudiants")


class DimEnseignant(Base):
    __tablename__ = "dim_enseignant"
    enseignant_id    = Column(Integer, primary_key=True)
    nom              = Column(String(150), nullable=False)
    grade            = Column(String(80))
    departement      = Column(String(100))
    email            = Column(String(150))
    date_recrutement = Column(Date)


class DimModule(Base):
    __tablename__ = "dim_module"
    module_id   = Column(Integer, primary_key=True, autoincrement=True)
    nom_module  = Column(String(150), nullable=False)
    filiere_id  = Column(Integer, ForeignKey("dim_filiere.filiere_id"))
    credits     = Column(SmallInteger, default=3)
    coefficient = Column(Numeric(3, 1), default=1.5)

    filiere = relationship("DimFiliere", back_populates="modules")


class DimSalle(Base):
    __tablename__ = "dim_salle"
    salle_id   = Column(Integer, primary_key=True)
    nom_salle  = Column(String(50), nullable=False)
    capacite   = Column(SmallInteger, nullable=False)
    type       = Column(String(50))
    batiment   = Column(String(50))
    disponible = Column(Boolean, default=True)


# ── Tables de faits ──────────────────────────────────────────────────────────

class FactInscription(Base):
    __tablename__ = "fact_inscriptions"
    inscription_id  = Column(Integer, primary_key=True, autoincrement=True)
    etudiant_id     = Column(Integer, ForeignKey("dim_etudiant.etudiant_id"))
    filiere_id      = Column(Integer, ForeignKey("dim_filiere.filiere_id"))
    temps_id        = Column(Integer, ForeignKey("dim_temps.temps_id"))
    annee           = Column(SmallInteger, nullable=False)
    statut          = Column(String(30))
    nb_inscriptions = Column(Integer, default=1)


class FactNote(Base):
    __tablename__ = "fact_notes"
    note_id     = Column(Integer, primary_key=True, autoincrement=True)
    etudiant_id = Column(Integer, ForeignKey("dim_etudiant.etudiant_id"))
    module_id   = Column(Integer, ForeignKey("dim_module.module_id"))
    filiere_id  = Column(Integer, ForeignKey("dim_filiere.filiere_id"))
    temps_id    = Column(Integer, ForeignKey("dim_temps.temps_id"))
    annee       = Column(SmallInteger, nullable=False)
    semestre    = Column(SmallInteger, nullable=False)
    note        = Column(Numeric(4, 2))
    mention     = Column(String(20))


class FactFinance(Base):
    __tablename__ = "fact_finances"
    finance_id       = Column(Integer, primary_key=True, autoincrement=True)
    temps_id         = Column(Integer, ForeignKey("dim_temps.temps_id"))
    annee            = Column(SmallInteger, nullable=False)
    poste_budgetaire = Column(String(150))
    type             = Column(String(20))
    montant          = Column(Numeric(15, 2), nullable=False)
    departement      = Column(String(100))
    description      = Column(Text)


class FactRecherche(Base):
    __tablename__ = "fact_recherche"
    projet_id      = Column(Integer, primary_key=True, autoincrement=True)
    enseignant_id  = Column(Integer, ForeignKey("dim_enseignant.enseignant_id"))
    temps_debut_id = Column(Integer, ForeignKey("dim_temps.temps_id"))
    departement    = Column(String(100))
    thematique     = Column(String(150))
    budget         = Column(Numeric(15, 2))
    depenses       = Column(Numeric(15, 2))
    statut         = Column(String(30))
    publications   = Column(SmallInteger, default=0)
