"""
generate_data.py
Génère des données fictives réalistes pour l'université UPES
et les sauvegarde en fichiers CSV dans le dossier data/.
"""

import os
import random
import csv
from datetime import date, timedelta

SEED = 42
random.seed(SEED)

BASE_DIR = os.path.join(os.path.dirname(__file__), "..", "data")
os.makedirs(BASE_DIR, exist_ok=True)

# ── Constantes ──────────────────────────────────────────────────────────────
FILIERES = [
    "Informatique", "Mathématiques", "Physique", "Chimie",
    "Biologie", "Génie Civil", "Génie Électrique", "Économie",
    "Droit", "Médecine",
]
GRADES = ["Professeur", "Maître de Conférences A", "Maître de Conférences B", "Maître Assistant A", "Maître Assistant B"]
DEPARTEMENTS = FILIERES[:]
TYPES_SALLE = ["Amphithéâtre", "Salle TD", "Labo Informatique", "Labo Sciences", "Salle Réunion"]
BATIMENTS = ["Bloc A", "Bloc B", "Bloc C", "Bloc D", "Bloc Sciences"]
STATUTS_ETUDIANT = ["Inscrit", "Diplômé", "Abandonné"]
STATUTS_RECHERCHE = ["En cours", "Terminé", "Suspendu"]
ANNEES = [2020, 2021, 2022, 2023, 2024]
SEMESTRES = [1, 2]

PRENOMS = ["Ahmed", "Mohamed", "Yacine", "Karim", "Sofiane", "Bilal", "Amine",
           "Sara", "Nadia", "Lina", "Meriem", "Asma", "Fatima", "Rania",
           "Omar", "Hamza", "Walid", "Imane", "Djamila", "Loubna"]
NOMS = ["Benali", "Boumediene", "Khelif", "Meziane", "Cherif", "Hadj",
        "Boudjelal", "Mansouri", "Rahmani", "Tebbal", "Guerfi", "Larbi",
        "Saidi", "Boukhari", "Benmoussa", "Amrani", "Ziani", "Ouali"]

MODULES_PAR_FILIERE = {
    "Informatique": ["Algorithmique", "Programmation C", "Bases de Données", "Réseaux", "Systèmes d'exploitation", "IA"],
    "Mathématiques": ["Analyse 1", "Algèbre 1", "Probabilités", "Statistiques", "Topologie", "Analyse Numérique"],
    "Physique": ["Mécanique", "Électromagnétisme", "Thermodynamique", "Optique", "Physique Quantique", "Relativité"],
    "Chimie": ["Chimie Générale", "Chimie Organique", "Chimie Analytique", "Thermochimie", "Électrochimie", "Biochimie"],
    "Biologie": ["Biologie Cellulaire", "Génétique", "Microbiologie", "Écologie", "Biochimie", "Physiologie"],
    "Génie Civil": ["Béton Armé", "Mécanique des Sols", "Hydraulique", "Topographie", "Matériaux", "Structures"],
    "Génie Électrique": ["Électronique", "Électrotechnique", "Automatique", "Signal", "Énergie", "Télécoms"],
    "Économie": ["Microéconomie", "Macroéconomie", "Comptabilité", "Finance", "Marketing", "Statistiques Éco"],
    "Droit": ["Droit Civil", "Droit Pénal", "Droit Commercial", "Droit Administratif", "Procédure Civile", "Droit Int."],
    "Médecine": ["Anatomie", "Physiologie", "Biochimie Méd.", "Pharmacologie", "Sémiologie", "Pathologie"],
}


def rand_name():
    return f"{random.choice(PRENOMS)} {random.choice(NOMS)}"


def rand_date(start_year=2018, end_year=2024):
    start = date(start_year, 1, 1)
    end = date(end_year, 12, 31)
    delta = (end - start).days
    return start + timedelta(days=random.randint(0, delta))


# ── 1. Étudiants ─────────────────────────────────────────────────────────────
def generate_etudiants(n=500):
    rows = []
    for i in range(1, n + 1):
        filiere = random.choice(FILIERES)
        annee_inscription = random.choice(ANNEES[:4])
        statut = random.choices(STATUTS_ETUDIANT, weights=[60, 30, 10])[0]
        rows.append({
            "etudiant_id": i,
            "nom": rand_name(),
            "filiere": filiere,
            "annee_inscription": annee_inscription,
            "statut": statut,
            "date_naissance": str(rand_date(1998, 2004)),
            "email": f"etudiant{i}@upes.dz",
            "sexe": random.choice(["M", "F"]),
        })
    return rows


# ── 2. Enseignants ────────────────────────────────────────────────────────────
def generate_enseignants(n=80):
    rows = []
    for i in range(1, n + 1):
        dept = random.choice(DEPARTEMENTS)
        modules = random.sample(MODULES_PAR_FILIERE.get(dept, ["Module Général"]), k=min(3, len(MODULES_PAR_FILIERE.get(dept, ["Module Général"]))))
        rows.append({
            "enseignant_id": i,
            "nom": rand_name(),
            "grade": random.choice(GRADES),
            "departement": dept,
            "cours_assignes": " | ".join(modules),
            "email": f"enseignant{i}@upes.dz",
            "date_recrutement": str(rand_date(2000, 2022)),
        })
    return rows


# ── 3. Notes ──────────────────────────────────────────────────────────────────
def generate_notes(etudiants, n_per_student=6):
    rows = []
    note_id = 1
    for etudiant in etudiants:
        filiere = etudiant["filiere"]
        modules = MODULES_PAR_FILIERE.get(filiere, ["Module Général"])
        annee = etudiant["annee_inscription"]
        for _ in range(n_per_student):
            module = random.choice(modules)
            note = round(random.gauss(11, 4), 2)
            note = max(0.0, min(20.0, note))
            semestre = random.choice(SEMESTRES)
            rows.append({
                "note_id": note_id,
                "etudiant_id": etudiant["etudiant_id"],
                "module": module,
                "filiere": filiere,
                "note": note,
                "semestre": semestre,
                "annee": annee,
                "mention": _mention(note),
            })
            note_id += 1
    return rows


def _mention(note):
    if note >= 16:
        return "Très Bien"
    if note >= 14:
        return "Bien"
    if note >= 12:
        return "Assez Bien"
    if note >= 10:
        return "Passable"
    return "Échec"


# ── 4. Finances ───────────────────────────────────────────────────────────────
POSTES_BUDGETAIRES = [
    "Salaires Enseignants", "Salaires Administratifs", "Équipements Informatiques",
    "Maintenance", "Bibliothèque", "Recherche Scientifique", "Énergie",
    "Travaux", "Activités Étudiantes", "Formation Continue",
]

def generate_finances(n=200):
    rows = []
    for i in range(1, n + 1):
        type_trans = random.choices(["Dépense", "Recette"], weights=[75, 25])[0]
        poste = random.choice(POSTES_BUDGETAIRES)
        if type_trans == "Recette":
            poste = random.choice(["Frais d'inscription", "Subvention État", "Contrats Recherche", "Dons"])
        montant = round(random.uniform(10_000, 5_000_000), 2)
        rows.append({
            "finance_id": i,
            "poste_budgetaire": poste,
            "montant": montant,
            "type": type_trans,
            "date": str(rand_date(2020, 2024)),
            "annee": random.choice(ANNEES),
            "departement": random.choice(DEPARTEMENTS + ["Administration Centrale"]),
            "description": f"{type_trans} - {poste}",
        })
    return rows


# ── 5. Salles ─────────────────────────────────────────────────────────────────
def generate_salles(n=60):
    rows = []
    for i in range(1, n + 1):
        type_salle = random.choice(TYPES_SALLE)
        capacite = {
            "Amphithéâtre": random.choice([100, 150, 200, 300]),
            "Salle TD": random.choice([30, 40, 50]),
            "Labo Informatique": random.choice([20, 30]),
            "Labo Sciences": random.choice([20, 25, 30]),
            "Salle Réunion": random.choice([10, 15, 20]),
        }[type_salle]
        rows.append({
            "salle_id": i,
            "nom_salle": f"Salle {i:03d}",
            "capacite": capacite,
            "type": type_salle,
            "batiment": random.choice(BATIMENTS),
            "disponible": random.choice([True, True, True, False]),
        })
    return rows


# ── 6. Recherche scientifique ─────────────────────────────────────────────────
THEMATIQUES = [
    "Intelligence Artificielle", "Énergies Renouvelables", "Biotechnologie",
    "Matériaux Avancés", "Réseaux & Télécoms", "Santé Numérique",
    "Économie Durable", "Droit Numérique",
]

def generate_recherche(enseignants, n=60):
    rows = []
    for i in range(1, n + 1):
        enseignant = random.choice(enseignants)
        budget = round(random.uniform(500_000, 10_000_000), 2)
        depense = round(random.uniform(0, budget), 2)
        statut = random.choice(STATUTS_RECHERCHE)
        publications = random.randint(0, 15) if statut == "Terminé" else random.randint(0, 5)
        rows.append({
            "projet_id": i,
            "titre": f"Projet {random.choice(THEMATIQUES)} #{i}",
            "enseignant_id": enseignant["enseignant_id"],
            "departement": enseignant["departement"],
            "budget": budget,
            "depenses": depense,
            "statut": statut,
            "date_debut": str(rand_date(2019, 2023)),
            "date_fin_prevue": str(rand_date(2023, 2026)),
            "publications": publications,
            "thematique": random.choice(THEMATIQUES),
        })
    return rows


# ── Writer CSV ────────────────────────────────────────────────────────────────
def write_csv(filename, rows):
    if not rows:
        return
    path = os.path.join(BASE_DIR, filename)
    with open(path, "w", newline="", encoding="utf-8") as f:
        writer = csv.DictWriter(f, fieldnames=rows[0].keys())
        writer.writeheader()
        writer.writerows(rows)
    print(f"  ✔ {filename} ({len(rows)} lignes) → {path}")


# ── Main ──────────────────────────────────────────────────────────────────────
if __name__ == "__main__":
    print("Génération des données fictives UPES...")

    etudiants = generate_etudiants(500)
    enseignants = generate_enseignants(80)
    notes = generate_notes(etudiants, n_per_student=6)
    finances = generate_finances(200)
    salles = generate_salles(60)
    recherche = generate_recherche(enseignants, n=60)

    write_csv("etudiants.csv", etudiants)
    write_csv("enseignants.csv", enseignants)
    write_csv("notes.csv", notes)
    write_csv("finances.csv", finances)
    write_csv("salles.csv", salles)
    write_csv("recherche.csv", recherche)

    print("\nDonnées générées avec succès dans le dossier data/")
